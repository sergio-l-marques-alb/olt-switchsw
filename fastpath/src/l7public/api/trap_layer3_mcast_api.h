/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename trap_layer3_mcast_api.h
*
* @purpose Trap Manager Layer 3 functions
*
* @component TrapMgr
*
* @comments none
*
* @created
*
* @author
*
* @end
*
**********************************************************************/
/********************************************************************
 *                   
 ********************************************************************/

#ifndef TRAP_LAYER3_MCAST_API_H
#define TRAP_LAYER3_MCAST_API_H

#define TRAPMGR_SPECIFIC_PIM_NEIGHBOR_LOSS                           18
#define TRAPMGR_SPECIFIC_DVMRP_NEIGHBOR_LOSS                         20
#define TRAPMGR_SPECIFIC_DVMRP_NEIGHBOR_NOT_PRUNING                  21

/* Begin Function Prototypes */

/*********************************************************************
*
* @purpose  A pimNeighborLoss trap signifies the loss of an adjacency
*           with a neighbor.  This trap should be generated when the
*           neighbor timer expires, and the router has no other
*           neighbors on the same interface with a lower IP address than
*           itself.
*
* @param    pimNeighborLossIfIndex       The IfIndex on which neighbor is lost.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/

L7_RC_t trapMgrPimTrapNeighborLoss(L7_uint32 pimNeighborLossIfIndex, L7_uint32 pimNeighborUpTime, L7_BOOL isPimSm);

/*********************************************************************
*
* @purpose  Allows a user to know whether the specified trap
*           is enabled or disabled
*
* @param    mode      L7_ENABLE/ L7_DISABLE
* @param    trapType  PIM trap type specified in L3_PIMDM_COMMDEFS_H
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t trapMgrPimTrapModeGet(L7_uint32 *mode, L7_uint32 trapType);

/*********************************************************************
*
* @purpose  Allows a user to enable or disable traps based on the
*           specified mode.
*
* @param    mode      L7_ENABLE/ L7_DISABLE
* @param    trapType  PIM trap type specified in L3_PIMDM_COMMDEFS_H
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrPimTrapModeSet(L7_uint32 mode, L7_uint32 trapType);

/*********************************************************************
*
* @purpose   A dvmrpNeighborLoss trap signifies the loss of a 2-way
*            adjacency with a neighbor.  This trap should be generated
*            when the neighbor state changes from active to one-way,
*            ignoring, or down.  The trap should be generated only if the
*            router has no other neighbors on the same interface with a
*            lower IP address than itself.
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
L7_RC_t trapMgrDvmrpTrapNeighborLoss(L7_uint32 dvmrpInfLocalAddress, L7_int32 dvmrpNeighborState);

/*********************************************************************
*
* @purpose   A dvmrpNeighborNotPruning trap signifies that a non-pruning
*            neighbor has been detected. This trap should be generated at
*            most once per generation ID of the neighbor.The trap
*            should be generated only if the router has no other neighbors on
*            the same interface with a lower IP address than itself.
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
L7_RC_t trapMgrDvmrpTrapNeighborNotPruning(L7_uint32 dvmrpInfLocalAddress, L7_uint32 dvmrpNeighborCapabilities);

/*********************************************************************
* @purpose  Returns Trap Manager's trap DVMRP state  
*
* @param    *mode     L7_ENABLE/L7_DISABLE
* @param    trapType  DVMRP trap type specified
*
* @returns  L7_SUCCESS
*           
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t trapMgrDvmrpTrapModeGet(L7_uint32 *mode, L7_uint32 trapType);

/*********************************************************************
* @purpose  Sets Trap Manager's DVMRP state  
*
*
* @param    mode      L7_ENABLE/L7_DISABLE
* @param    trapType  DVMRP trap type specified
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t trapMgrDvmrpTrapModeSet(L7_uint32 mode, L7_uint32 trapType);

/* End Function Prototypes */

#endif /* TRAP_LAYER3_MCAST_API_H */
