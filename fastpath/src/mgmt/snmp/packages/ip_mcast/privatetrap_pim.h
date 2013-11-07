/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name: privatetrap_pim.h
*
* Purpose: PIM trap functions
*
* Created by: Abdul Shareef (BRI) 02/05/2002
*
* Component: SNMP
*
*********************************************************************/

/*********************************************************************
                         
**********************************************************************
*********************************************************************/

#ifndef PRIVATETRAP_PIM_H
#define PRIVATETRAP_PIM_H

#include "l7_common.h"

/*********************************************************************
*
* @purpose  A pimNeighborLoss trap signifies the loss of an adjacency
*           with a neighbor.  This trap should be generated when the
*           neighbor timer expires, and the router has no other
*           neighbors on the same interface with a lower IP address than
*           itself.
*
* @param    pimNeighborIfIndex      The lost neighbor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    // shareef
*
* @end
*********************************************************************/
L7_RC_t snmp_pimNeighborLossTrapSend(L7_uint32 pimNeighborUpTime);

#endif /* PRIVATETRAP_PIM_H */
