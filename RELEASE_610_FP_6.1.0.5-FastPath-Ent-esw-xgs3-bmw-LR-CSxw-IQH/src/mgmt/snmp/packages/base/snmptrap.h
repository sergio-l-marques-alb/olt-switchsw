/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename snmptrap.h
*
* @purpose Header information for calling SNMP traps
*
* @component SNMP
*
* @comments none
*
* @create 1/10/2001
*
* @author cpverne
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/



#ifndef SNMPTRAP_H
#define SNMPTRAP_H

#include <l7_common.h>

/* Begin Function Prototypes */

/*********************************************************************
*
* @purpose  Send a Cold Start Trap
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
ColdStartTrapSend ();

/*********************************************************************
*
* @purpose  Send a Warm Start Trap
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if error
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
WarmStartTrapSend ();

/*********************************************************************
*
* @purpose  Send a Link Down Trap
*
* @param    ifIndex     External ifIndex of link that went down.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if error
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
LinkDownTrapSend ( L7_uint32 ifIndex );


/*********************************************************************
*
* @purpose  Send a Link Up Trap
*
* @param    ifIndex     External ifIndex of link that came up.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if error
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
LinkUpTrapSend ( L7_uint32 ifIndex );

/*********************************************************************
*
* @purpose  Send Authentication failure
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if error
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
AuthenFailureTrapSend ();

/* End Function Prototypes */

#endif /* SNMPTRAP_H */
