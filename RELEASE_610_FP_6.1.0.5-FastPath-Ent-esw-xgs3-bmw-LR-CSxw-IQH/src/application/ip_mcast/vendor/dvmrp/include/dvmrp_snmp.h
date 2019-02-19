/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_snmp.h
*
* @purpose Contains structures,constants,macros, for dvmrp protocol
*
* @component
*
* @comments
*
* @create 07/03/2006
*
* @author Prakash/Shashidhar
* @end
*
**********************************************************************/
#ifndef _DVMRP_SNMP_H_
#define _DVMRP_SNMP_H_
#include <dvmrp_common.h>

/*********************************************************************
* @purpose  This function is used to send the trap when the neighbour 
*           loss happens
*           
* @param    interface  - @b{(input)}Pointer to the Interface on which the neighbour 
*                                    loss happened
*                lostNbr    -  @b{(input)} Pointer to the nbr lost
*                newstate   -@b{(input)} new state of the neighbour

*           
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    None
*       
* @end
*********************************************************************/
L7_RC_t dvmrp_send_neighbor_loss_trap(dvmrp_interface_t *interface, 
                                      dvmrp_neighbor_t *lostNbr, 
                                      L7_int32 newState);

/*********************************************************************
* @purpose  This function is used to send the trap when the neighbour 
*           is not pruning a perticular src network
*           
* @param    interface           -            @b{(input)}Pointer to the Interface on which the neighbour
*                                                             pruning not happening
* @param    notPruningNbr   -            @b{(input)}Pointer to the nbr where pruning is not happening
* @param    capabilities        -            @b{(input)} capabilities of the neighbour
* 
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    None
*       
* @end
*********************************************************************/
L7_RC_t dvmrp_send_neighbor_not_pruning_trap(dvmrp_interface_t *interface, 
                                             dvmrp_neighbor_t *notPruningNbr, 
                                             L7_ulong32 capabilities);
#endif
