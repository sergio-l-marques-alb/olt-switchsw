/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name: privatetrap_dvmrp.h
*
* Purpose: DVMRP trap functions
*
* Created by: Mahe Korukonda (BRI) 03/19/2002
*
* Component: SNMP
*
*********************************************************************/

/*********************************************************************

**********************************************************************
*********************************************************************/

#ifndef PRIVATETRAP_DVMRP_H
#define PRIVATETRAP_DVMRP_H

#include "l7_common.h"

L7_RC_t snmp_dvmrpNeighborLossTrapSend(L7_uint32 dvmrpInfLocalAddress, L7_uint32 dvmrpNeighborState);
L7_RC_t snmp_dvmrpNeighborNotPruningTrapSend(L7_uint32 dvmrpInfLocalAddress, L7_uint32 dvmrpNeighborCapabilities);

#endif /* PRIVATETRAP_DVMRP_H */
