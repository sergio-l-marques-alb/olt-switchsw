/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename    dvmrp_vend_ctrl.h
*
* @purpose     DVMRP vendor-specific internal definitions
*
* @component   DVMRP Mapping Layer
*
* @comments    none
*
* @create      02/16/2002
*
* @author      M Pavan K Chakravarthi
*
* @end
*
**********************************************************************/

#ifndef _DVMRP_VEND_CTRL_H_
#define _DVMRP_VEND_CTRL_H_

/*********************************************************************
* @purpose  TO get the corresponding control block.
*
* @param    familyType     @b{(input)}      Address Family type
*
* @returns  the pointer to the control block
*
* @notes
*
* @end
*********************************************************************/
MCAST_CB_HNDL_t dvmrpMapProtocolCtrlBlockGet(L7_uchar8 familyType);
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
* @commentsnone
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapDvmrpTrapNeighborNotPruning(L7_inet_addr_t *ipAddr,
                                            L7_uint32 neighborCaps);
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
* @param    neighborState  @b{(input)} The state of the neighbor
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
                                      L7_int32 neighborState);

#endif /* _DVMRP_VEND_CTRL_H_ */
