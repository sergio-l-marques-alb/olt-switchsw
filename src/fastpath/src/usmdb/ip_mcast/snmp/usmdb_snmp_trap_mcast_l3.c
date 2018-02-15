/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename src\usmdb\usmdb_snmp_trap_mcast_l3.c
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
#include "l7_common.h"
#include "usmdb_snmp_trap_mcast_api_l3.h"
#include "snmp_trap_api_mcast_l3.h"

/* Begin Function Declarations: usmdb_snmp_trap_api_mcast_l3.h */

L7_RC_t usmDbSnmpDvmrpTrapNeighborNotPruningTrapSend(L7_uint32 UnitIndex, L7_uint32 dvmrpInfLocalAddress, L7_uint32 dvmrpNeighborCapabilities)
{
#ifndef L7_SNMP_PACKAGE
    return L7_NOT_SUPPORTED;
#else
    return SnmpDvmrpNeighborNotPruningTrapSend(dvmrpInfLocalAddress, dvmrpNeighborCapabilities);
#endif
}

L7_RC_t usmDbSnmpDvmrpTrapNeighborLossTrapSend(L7_uint32 UnitIndex, L7_uint32 dvmrpInfLocalAddress, L7_int32 dvmrpNeighborState)
{
#ifndef L7_SNMP_PACKAGE
    return L7_NOT_SUPPORTED;
#else
    return SnmpDvmrpNeighborLossTrapSend(dvmrpInfLocalAddress, dvmrpNeighborState);
#endif
}
             
/*********************************************************************
*
* @purpose  Send a trap when PIM neighbor adjacency is lost
*
* @param    UnitIndex L7_uint32 the unit for this operation
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
L7_RC_t usmDbSnmpPimNeighborLossTrapSend(L7_uint32 UnitIndex, L7_uint32 pimNeighborLossIfIndex, L7_uint32 pimNeighborUpTime, L7_BOOL isPimSm)
{
#ifndef L7_SNMP_PACKAGE
    return L7_NOT_SUPPORTED;
#else
    return SnmpPimNeighborLossTrapSend(pimNeighborUpTime);
#endif
}

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
L7_RC_t usmDbSnmpDvmrpNeighborNotPruningTrapSend(L7_uint32 UnitIndex, L7_uint32 dvmrpInfLocalAddress, L7_uint32 dvmrpNeighborCapabilities)
{
#ifndef L7_SNMP_PACKAGE
    return L7_NOT_SUPPORTED;
#else
    return SnmpDvmrpNeighborNotPruningTrapSend(dvmrpInfLocalAddress, dvmrpNeighborCapabilities);
#endif
}

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
L7_RC_t usmDbSnmpDvmrpNeighborLossTrapSend(L7_uint32 UnitIndex, L7_uint32 dvmrpInfLocalAddress, L7_int32 dvmrpNeighborState)
{
#ifndef L7_SNMP_PACKAGE
    return L7_NOT_SUPPORTED;
#else
    return SnmpDvmrpNeighborLossTrapSend(dvmrpInfLocalAddress, dvmrpNeighborState);
#endif
}

