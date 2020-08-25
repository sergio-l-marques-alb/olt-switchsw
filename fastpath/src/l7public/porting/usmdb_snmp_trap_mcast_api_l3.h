/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename src\usmdb\usmdb_snmp_trap_mcast_api_l3.h
*
* @purpose Provide interface to SNMP Trap API's for unitmgr components
*
* @component
*
* @comments
*
* @create
*
* @author
* @end
*
**********************************************************************/
/*********************************************************************
 *                    
 *********************************************************************/

#ifndef USMDB_SNMP_TRAP_MCAST_API_L3_H
#define USMDB_SNMP_TRAP_MCAST_API_L3_H

#include "l7_common.h"

/* Begin Function Prototypes */

L7_RC_t usmDbSnmpDvmrpTrapNeighborNotPruningTrapSend(L7_uint32 UnitIndex, L7_uint32 dvmrpInfLocalAddress, L7_uint32 dvmrpNeighborCapabilities);

L7_RC_t usmDbSnmpDvmrpTrapNeighborLossTrapSend(L7_uint32 UnitIndex, L7_uint32 dvmrpInfLocalAddress, L7_int32 dvmrpNeighborState);

L7_RC_t usmDbSnmpPimNeighborLossTrapSend(L7_uint32 UnitIndex, L7_uint32 pimNeighborLossIfIndex, L7_uint32 pimNeighborUpTime, L7_BOOL isPimSm);

L7_RC_t usmDbSnmpDvmrpNeighborNotPruningTrapSend(L7_uint32 UnitIndex, L7_uint32 dvmrpInfLocalAddress, L7_uint32 dvmrpNeighborCapabilities);

L7_RC_t usmDbSnmpDvmrpNeighborLossTrapSend(L7_uint32 UnitIndex, L7_uint32 dvmrpInfLocalAddress, L7_int32 dvmrpNeighborState);

/* End Function Prototypes */

#endif /* USMDB_SNMP_TRAP_MCAST_API_L3_H */
