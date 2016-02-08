/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name: snmp_trap_api__mcast_l3.h
*
* Purpose: API interface for SNMP Agent Traps
*
* Created by:
*
* Component: SNMP
*
*********************************************************************/
/********************************************************************
 *                    
 *******************************************************************/

#ifndef SNMP_TRAP_API_MCAST_L3_H
#define SNMP_TRAP_API_MCAST_L3_H

#include "l7_common.h"

/* Begin Function Prototypes */
/*********************************************************************
*
* @purpose  Send a trap when PIM neighbor adjacency is lost
*
*
* @param    pimNeighborLossIfIndex       The interface on which neighbor is lost
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t SnmpPimNeighborLossTrapSend(L7_uint32 pimNeighborIfIndex, L7_uint32 neighborUpTime, L7_BOOL isPimSm);

/*********************************************************************
*
* @purpose  Send a trap when DVMRP neighbor is not pruning
*
*
* @param    dvmrpInfLocalAddress       The interface address on which neighbor is lost
* @param    dvmrpNeighborCapabilities  The Capabilities of the neighbor
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t SnmpDvmrpNeighborNotPruningTrapSend(L7_uint32 dvmrpInfLocalAddress, L7_uint32 dvmrpNeighborCapabilities);

/*********************************************************************
*
* @purpose  Send a trap when DVMRP neighbor adjacency is lost
*
*
* @param    dvmrpInfLocalAddress       The interface address on which neighbor is lost
* @param    dvmrpNeighborState       The state of the neighbor
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t SnmpDvmrpNeighborLossTrapSend(L7_uint32 dvmrpInfLocalAddress, L7_int32 dvmrpNeighborState);

/* End Function Prototypes */

#endif /* SNMP_TRAP_API_MCAST_L3_H */
