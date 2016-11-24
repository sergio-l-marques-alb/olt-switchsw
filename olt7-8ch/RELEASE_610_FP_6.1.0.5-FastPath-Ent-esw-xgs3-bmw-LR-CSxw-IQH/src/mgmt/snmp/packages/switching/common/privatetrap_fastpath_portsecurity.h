/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: privatetrap_fastpath_portsecurity..h
*
* Purpose: Port Security related trap functions
*
* Created by: kmanish 06/18/2004
*
* Component: SNMP
*
*********************************************************************/

#ifndef PRIVATETRAP_FASTPATH_PORTSECURITY_H
#define PRIVATETRAP_FASTPATH_PORTSECURITY_H

/* Begin Function Prototypes */

/*********************************************************************
*
* @purpose  Sends when a packet is received on a locked port with a 
*           source MAC address that is not allowed.
*
* @param    intIfNum       Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Creates an SNMP trap OID and a VarBind structure.
*
* @end
*********************************************************************/
L7_RC_t snmp_agentPortSecurityViolationTrapSend(L7_uint32 intIfNum);

/* End Function Prototypes */

#endif /*PRIVATETRAP_FASTPATH_PORTMACLOCKING_H  */
