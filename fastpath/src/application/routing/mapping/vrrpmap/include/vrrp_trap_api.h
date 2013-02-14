/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  vrrp_trap_api.c
*
* @purpose   vrrp Trap functions
*
* @component vrrp Mapping Layer
*
* @comments  
*            
*
* @create    12/12/2001
*
* @author    Kumar Manish
*
* @end
*             
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/
#ifndef _VRRP_TRAP_API_H_
#define _VRRP_TRAP_API_H_

/*
***********************************************************************
*                     VRRP TRAP FUNCTIONS
***********************************************************************
*/

/*---------------------------------------------------------------------------*/

/* Functions to notify a network manager of potentially critical VRRP      */
/* events with SNMP traps. These functions issue vrrp traps by invoking the       */
/* LVL7 Trap manager which in turn calls SNMP to send the traps.             */
/*---------------------------------------------------------------------------*/

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
L7_RC_t vrrpMapTrapNewMasterTrap(L7_uint32 vrrpOperMasterIpAddr);

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
L7_RC_t vrrpMapTrapAuthFailureTrap(L7_uint32 vrrpTrapPacketSrc, L7_int32 vrrpTrapAuthErrorType);


#endif /* _VRRP_TRAP_API_H_ */

