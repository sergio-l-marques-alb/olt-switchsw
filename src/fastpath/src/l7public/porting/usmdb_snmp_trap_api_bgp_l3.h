
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename src\usmdb\usmdb_snmp_trap_api_l3.h
*
* @purpose Provide interface to SNMP Trap API's for unitmgr components
*
* @component SNMP
*
* @comments tba
*
* @create 06/21/2001
*
* @author cpverne
* @end
*
**********************************************************************/

#ifndef USMDB_SNMP_TRAP_API_BGP_L3_H
#define USMDB_SNMP_TRAP_API_BGP_L3_H

#include "l7_common.h"
#include "usmdb_snmp_trap_api_l3.h"

/* Begin Function Prototypes */

/*********************************************************************
*
* @purpose  Signifies that the BGP FSM  has transitioned to the
*           Established state.
*
* @param    UnitIndex The unit for this operation
* @param    peerId    Peer Identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpBgpTrapBgpEstablishedTrapSend(L7_uint32 UnitIndex, L7_uint32 peerRemoteAddr, L7_char8* peerLastError, L7_uint32 peerState);

/*********************************************************************
*
* @purpose  Signifies that BGP FSM moves from a higher numbered state
*           to a lower numbered state.
*
* @param    UnitIndex The unit for this operation
* @param    peerId              Peer Identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpBgpTrapBgpBackwardTransitionTrapSend(L7_uint32 UnitIndex, L7_uint32 peerRemoteAddr, L7_char8* peerLastError, L7_uint32 peerState);

/* End Function Prototypes */

#endif /* USMDB_SNMP_TRAP_API_L3_H */
