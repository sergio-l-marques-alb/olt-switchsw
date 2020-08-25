/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename  dvmrp_vend_ctrl.c
*
* @purpose   DVMRP vendor-specific control routines
*
* @component DVMRP Mapping Layer
*
* @comments  none
*
* @create    02/17/2002
*
* @author    M Pavan K Chakravarthi
*
* @end
*
**********************************************************************/
#include "l7_common_l3.h"
#include "l3_mcast_commdefs.h"
#include "dvmrp_api.h"                   
#include "l7_ip_api.h"                   
#include "l3_mcast_default_cnfgr.h"
#include "dvmrp_cnfgr.h" 
#include "dvmrp_config.h" 
#include "dvmrp_map_util.h" 
#include "dvmrp_map_debug.h"
#include "trap_layer3_mcast_api.h"
/*********************************************************************
*
* @purpose   A dvmrpNeighborLoss trap signifies the loss of a 2-way
*            adjacency with a neighbor.  This trap should be generated
*            when the neighbor state changes from active to one-way,
*            ignoring, or down.  The trap should be generated only if the
*            router has no other neighbors on the same interface with a
*            lower IP address than itself.
*
* @param    ipAddr         @b{(input)} The interface address on which neighbor is lost
* @param    neighborState @b{(input)} The state of the neighbor
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapDvmrpTrapNeighborLoss(L7_inet_addr_t *ipAddr,
                                      L7_int32 neighborState)
{
  L7_uint32 trapIpAddr;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (inetAddressGet(L7_AF_INET, ipAddr, &trapIpAddr) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES, "Failed to get IPV4/IPV6 address\n");
    return L7_FAILURE;
  }

  return trapMgrDvmrpTrapNeighborLoss(trapIpAddr, neighborState);
}
/*********************************************************************
*
* @purpose   A dvmrpNeighborNotPruning trap signifies that a non-pruning
*            neighbor has been detected. This trap should be generated at
*            most once per generation ID of the neighbor.The trap
*            should be generated only if the router has no other neighbors on
*            the same interface with a lower IP address than itself.
*
* @param    ipAddr        @b{(input)} The interface address on which neighbor is lost
* @param    neighborCaps  @b{(input)} The Capabilities of the neighbor
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapDvmrpTrapNeighborNotPruning(L7_inet_addr_t *ipAddr,
                                            L7_uint32 neighborCaps)
{
  L7_uint32 trapIpAddr;

  DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_APIS,"\nFunction Entered\n");
  if (inetAddressGet(L7_AF_INET, ipAddr, &trapIpAddr) != L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES, "Failed to get IPV4/IPV6 address\n");
    return L7_FAILURE;
  }

  return trapMgrDvmrpTrapNeighborNotPruning(trapIpAddr, neighborCaps);
}
/*********************************************************************
* @purpose  TO get the corresponding control block.
*
* @param    familyType     @b{(input)}      Address Family type
*
* @returns  the pointer to the control block
*
* @comments
*
* @end
*********************************************************************/
MCAST_CB_HNDL_t dvmrpMapProtocolCtrlBlockGet(L7_uchar8 familyType)
{
  return dvmrpGblVar_g.dvmrpCb;
}

