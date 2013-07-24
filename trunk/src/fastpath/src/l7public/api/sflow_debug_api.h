/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    sflow_debug_api.h
* @purpose     sFLow debug functions
* @component   sFlow
* @comments    none
* @create      12/11/2007
* @author      drajendra
* @end
*             
**********************************************************************/
#ifndef SFLOW_DEBUG_API_H
#define SFLOW_DEBUG_API_H

#include "l7_common.h"
/************************************************************************
* @purpose  Get the current status of displaying sFlow packet debug info
*
* @param    transmitFlag  @b{(output)}  Tx Debug Flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
************************************************************************/
L7_RC_t sFlowDebugPacketTraceFlagGet(L7_BOOL *transmitFlag);
/*****************************************************************************
* @purpose  Turns on/off the displaying of sFlow packet debug info of
*           a sFlow agent
*
* @param    transmitFlag  @b{(input)}  Tx Debug Flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*****************************************************************************/
L7_RC_t sFlowDebugPacketTraceFlagSet(L7_BOOL transmitFlag);

#endif /* SFLOW_DEBUG_API_H */
