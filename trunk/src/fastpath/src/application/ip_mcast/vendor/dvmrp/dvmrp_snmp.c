/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename dvmrp_snmp.c
*
* @purpose Contains routines to provide management of DVMRP protocol
*
* @component
*
* @comments
*
* @create 06/03/2006
*
* @author Prakash/Shashidhar
* @end
**********************************************************************/
#include "dvmrp_snmp.h"
#include "dvmrp_vend_ctrl.h"

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
                                      L7_int32 newState)
{
  dvmrp_neighbor_t *nbr = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  /* Don't send trap if one of the neighbors on this interface has a 
   * lower IP addr than ours */
  for (nbr = (dvmrp_neighbor_t*)SLLFirstGet(&(interface->ll_neighbors));
      nbr != L7_NULLPTR;
      nbr = (dvmrp_neighbor_t*)SLLNextGet(&(interface->ll_neighbors),(void*)nbr))
  {
    if ((L7_INET_ADDR_COMPARE(&interface->primary.inetAddr.addr, 
                              &nbr->nbrAddr.addr) > L7_NULL) && (nbr != lostNbr))
    {
      return rc;
    }
  }
  rc = dvmrpMapDvmrpTrapNeighborLoss(&interface->primary.inetAddr.addr, newState);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return rc;
}

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
                                             L7_ulong32 capabilities)
{
  dvmrp_neighbor_t *nbr =  L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  /* Don't send trap if one of the neighbors on this interface has a lower 
   * IP addr than ours */
  for (nbr = (dvmrp_neighbor_t*)SLLFirstGet(&(interface->ll_neighbors));
      nbr != L7_NULLPTR;
      nbr = (dvmrp_neighbor_t*)SLLNextGet(&(interface->ll_neighbors),(void*)nbr))
  {
    if ((L7_INET_ADDR_COMPARE(&interface->primary.inetAddr.addr, 
                              &nbr->nbrAddr.addr) > L7_NULL) &&  (nbr != notPruningNbr))
    {
      return rc;
    }
  }
  rc = dvmrpMapDvmrpTrapNeighborNotPruning(&interface->primary.inetAddr.addr,capabilities);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return rc;
}

