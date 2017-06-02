/*********************************************************************
*                                                                 
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: dtl_acl.h
*
* Purpose: This file contains the functions to setup AUTO-VOIP
*          management calls to the driver.
*
* Component: Device Transformation Layer (DTL)
*
* Comments: 
*
* Created by: Murali krishna Peddireddy
*
*********************************************************************/
#ifndef DTL_VOIP_H
#define DTL_VOIP_H

#include "voip_parse.h"

/*********************************************************************
* @purpose  Add/Delete VOIP profile 
*
* @param    intIfNum       @b{(input)} Interface number to apply to
* @param    mode           @b{(input)} Enable/Disable
* @param    bandWidth      @b{(input)} Min bandwidth 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t dtlVoipProfileSet(L7_uint32 intIfNum,L7_BOOL mode, L7_uint32 bandWidth);
/*********************************************************************
* @purpose  Add/Delete VOIP calls 
*
* @param    intIfNum       @b{(input)} Interface number to apply to
* @param    mode           @b{(input)} Enable/Disable
* @param    voipFpEntry_t  @b{(input)} VOIP entry parameters
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t dtlVoipProfileParamSet(L7_BOOL val,
                               L7_uint32 protocol,L7_uint32 srcIp,
                               L7_uint32 dstIp, L7_ushort16 srcL4port,
                               L7_ushort16 dstL4port);

L7_RC_t dtlVoipSessionStatsGet(L7_uint32 srcIp,
                               L7_uint32 dstIp,
                               L7_ushort16 srcL4port,
                               L7_ushort16 dstL4port,
                               L7_ulong64  *pHitcount);

#endif

