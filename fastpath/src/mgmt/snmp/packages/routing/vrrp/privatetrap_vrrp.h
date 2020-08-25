
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: privatetrap_vrrp.h
*
* Purpose: VRRP trap functions
*
* Created by: Mike Fiorito 10/26/2001
*
* Component: SNMP
*
*********************************************************************/

/*********************************************************************
                         
**********************************************************************
*********************************************************************/

#ifndef PRIVATETRAP_VRRP_H
#define PRIVATETRAP_VRRP_H

#include "l7_common.h"

/* Begin Function Prototypes */

/*********************************************************************
*
* @purpose  Signifies that the sending agent has transitioned to the 
*           'Master' state.
*
* @param    vrrpOperMasterIpAddr   The master router's real (primary) IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t snmp_vrrpTrapNewMasterTrapSend(L7_uint32 vrrpOperMasterIpAddr);


/*********************************************************************
*
* @purpose  Signifies that a packet has been received from a router
*           whose authentication key or authentication type conflicts
*           with this router's authentication key or authentication type.
*
* @param    vrrpTrapPacketSrc       The address of an inbound VRRP packet
* @param    vrrpTrapAuthErrorType   The type of configuration conflict
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t snmp_vrrpTrapAuthFailureTrapSend(L7_uint32 vrrpTrapPacketSrc, 
                                         L7_int32 vrrpTrapAuthErrorType);

#endif /* PRIVATETRAP_VRRP_H */
