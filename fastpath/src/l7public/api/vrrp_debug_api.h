/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename vrrp_debug_api.h
*
* @purpose vrrp APIs
*
* @component vrrp
*
* @comments none
*
* @create 
*
* @author 
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/


#ifndef VRRP_DEBUG_API_H
#define VRRP_DEBUG_API_H

#include "l7_common.h"
#include "statsapi.h"

#include "comm_structs.h"

/*********************************************************************
* @purpose  Get the current status of displaying vrrp packet debug info
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL vrrpDebugPacketTraceFlagGet();

/*********************************************************************
* @purpose  Turns on/off the displaying of vrrp packet debug info
*
* @param    flag         new value of the Packet Debug flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t vrrpDebugPacketTraceFlagSet(L7_BOOL flag);

#endif


