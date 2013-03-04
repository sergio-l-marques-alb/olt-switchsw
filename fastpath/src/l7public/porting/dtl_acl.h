/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: dtl_bw.c
*
* Purpose: This file contains the functions to setup ACL    
*          management calls to the driver.
*
* Component: Device Transformation Layer (DTL)
*
* Commnets: 
*
* Created by: djohnson 7/22/2002 
*
*********************************************************************/
#ifndef INCLUDE_DTLCTRL_QOS_ACL_H
#define INCLUDE_DTLCTRL_QOS_ACL_H

#include "dapi.h"

/*********************************************************************
* @purpose  Create a new access list
*
* @param    intIfNum       @b{(input)} Interface number to apply to
* @param    direction      @b{(input)} Direction to filter traffic
* @param    pTLV           @b{(input)} ptr to first type-length pair
* @param    tlvTotalSize   @b{(input)} number of TLVs
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Adding a (aclnum, interface, direction) instance
*
* @end
*********************************************************************/
L7_RC_t dtlQosAclCreate(L7_uint32 intIfNum, L7_uint32 direction, L7_uchar8 *pTLV, L7_uint32 tlvTotalSize);

/*********************************************************************
* @purpose  Delete an access list
*
* @param    intIfNum       @b{(input)} Interface number
* @param    direction      @b{(input)} Direction
* @param    pTLV           @b{(input)} ptr to first type-length pair
* @param    tlvTotalSize   @b{(input)} number of TLVs
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Removes an (aclnum, interface, direction) instance
*
* @end
*********************************************************************/
L7_RC_t dtlQosAclDelete(L7_uint32 intIfNum, L7_uint32 direction, L7_uchar8 *pTLV, L7_uint32 tlvTotalSize);

/*********************************************************************
* @purpose  Create a new access list
*
* @param    direction      @b{(input)} Direction to filter traffic
* @param    pTLV           @b{(input)} ptr to first type-length pair
* @param    tlvTotalSize   @b{(input)} number of TLVs
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Adding a (aclnum, direction) instance
*
* @end
*********************************************************************/
L7_RC_t dtlQosSysAclCreate(L7_uint32 direction, L7_uchar8 *pTLV, L7_uint32 tlvTotalSize);

/*********************************************************************
* @purpose  Delete an access list
*
* @param    direction      @b{(input)} Direction
* @param    pTLV           @b{(input)} ptr to first type-length pair
* @param    tlvTotalSize   @b{(input)} number of TLVs
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Removes an (aclnum, direction) instance
*
* @end
*********************************************************************/
L7_RC_t dtlQosSysAclDelete(L7_uint32 direction, L7_uchar8 *pTLV, L7_uint32 tlvTotalSize);

/*********************************************************************
* @purpose  Request an ACL logging rule hit counter from the device
*
* @param    correlator     @b{(input)} ACL rule log correlator
* @param    *pHitCount     @b{(input)} Hit counter output ptr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST   Correlator not found in hardware
*
* @notes    Don't panic if this call fails, since it probably means
*           a correlator known by the application did not get applied
*           in the device (perhaps it ran out of counter resources).
*
* @notes    This call is not for a specific interface.  If the 
*           hardware maintains separate counters per interface for
*           a given correlator, then it is assumed that HAPI will
*           output the sum total value.
*
* @end
*********************************************************************/
L7_RC_t dtlQosAclRuleCountGet(L7_uint32 correlator, L7_ulong64 *pHitCount);
/*********************************************************************
* @purpose  Set the ACL rule status
*
* @param    correlator @b{(input)} Time based ACL rule correlator
* @param    status     @b{(input)} status of the rule
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST   Correlator not found in hardware, rule not installed
*
* @end
*********************************************************************/
L7_RC_t dtlQosAclRuleStatusSet(L7_uint32 correlator, L7_ushort16 status);
#endif
