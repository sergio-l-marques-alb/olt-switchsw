/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_nbr.h
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
#ifndef _DVMRP_NBR_H
#define _DVMRP_NBR_H
#include <dvmrp_common.h>


#define DVMRP_VIF_DOWN           0x10    /* kernel state of interface */
#define DVMRP_VIF_QUERIER        0x40    /* I am the subnet's querier */
#define DVMRP_OLD_VER_NEIGHBOR_EXPIRE_TIME      125 /* should be greater 
                                                       than hold time*/

/*********************************************************************
* @purpose  This function processes the DVMRP ask neighbours packet
*
* @param    from_if -  @b{(input)} Pointer to the Interface throufh which the ask neighbours 
*                                    packet has been recieved.  
* @param    from    -  @b{(input)}Pointer to the source address of the packet
* 
* @returns  L7_SUCCESS/L7_FAILURE.
*
* @notes    This function sends the response packet to the sender 
*           of the request packet
* @end
*********************************************************************/
L7_RC_t dvmrp_neighbors2 (dvmrp_interface_t *from_if, L7_dvmrp_inet_addr_t *from);

/*********************************************************************
* @purpose  This function is used to delete the nbr node. 
*
* @param    nbr - -  @b{(input)}Pointer to the nbr node.

* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
L7_RC_t dvmrp_neighbor_delete(L7_sll_member_t *pNode);

/*********************************************************************
* @purpose  This function is used to get the nbr node based on the 
*           source address. 
*
* @param    interface            -  @b{(input)} Interfae on which the neighbor node 
*                                                     is searched.
* @param    versionSupport   -  @b{(input)} Pointer to the version supported. 
* @param    source               -  @b{(input)} source address  
*
* @returns  Neighbour node
*
* @notes    None
*       
* @end
*********************************************************************/
dvmrp_neighbor_t *dvmrp_neighbor_lookup(dvmrp_interface_t *interface,
                                        L7_ulong32 versionSupport, L7_dvmrp_inet_addr_t *source);


/*********************************************************************
* @purpose  This function is used to add the new neighbour to the neighbour 
*           list. 
*
* @param    interface            - @b{(input)} Pointer to the Interfae for which the 
*                                                          new neighbour is added.
* @param    versionSupport   - @b{(input)} versionSupport supported by 
*                                                    neighbour.
* @param    genid                -  @b{(input)} Generation id 
* @param    source              -  @b{(input)} source address of the neighbour

* @returns  Neighbour node
*
* @notes    None
*       
* @end
*********************************************************************/
dvmrp_neighbor_t *dvmrp_neighbor_register(dvmrp_interface_t *interface, 
                                          L7_ulong32 versionSupport, L7_ulong32 genid, 
                                          L7_dvmrp_inet_addr_t *source);

/*********************************************************************
* @purpose  This function is used to recover the neighbour which has 
*           been set to be deleted. 
*
* @param    interface -  @b{(input)}Pointer to the interfae for which the new neighbour is 
*                                    added.
* @param    nbr      -  @b{(input)}Pointer to the neighbour node
* 
* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
void dvmrp_neighbor_recover (dvmrp_interface_t *interface, 
                             dvmrp_neighbor_t *nbr);

/*********************************************************************
* @purpose  This function is used to know whether this is the only 
*           dependent neighbour
*
* @param    bitset  -  @b{(input)} Pointer to the neighbour bit set
*                 nbr     -  @b{(input)} Pointer to the nbr node to be verified
*           
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes    None
*       
* @end
*********************************************************************/
L7_int32 dvmrp_is_only_neighbor_dependent(neighbor_bitset_t *bitset, 
                                          dvmrp_neighbor_t *nbr);

/*********************************************************************
* @purpose  This function is used to know whether any other dependent 
*           neighbours on this interface
*
* @param    bitset    -   @b{(input)} neighbour bit set
* @param    interface -   @b{(input)} interface for which the dependent 
*                            neighbours to be verified
*                            
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None
*       
* @end
*********************************************************************/
L7_int32 dvmrp_any_dependents_on_this_interface(neighbor_bitset_t *bitset,
                                                dvmrp_interface_t *interface);


/*********************************************************************
* @purpose  This function is used to elect the DF 
*
* @param   route    -  @b{(input)}Pointer to the route entry 
*                nbr      -   @b{(input)}Pointer to the neighbor node
*                metric  -   @b{(input)}Pointer to the metric
* @notes    None
*       
* @end
*********************************************************************/
void dvmrp_df_elect(dvmrp_route_t *route, dvmrp_neighbor_t* nbr, 
                    L7_int32 metric);
/*********************************************************************
* @purpose  This function does the cleanup action for nbr node. 
*
* @param    nbr    -   @b{(input)} pointer to the neighbor node.
* 
* @returns  None
*
* @notes    nbr node is removed.
*       
* @end
*********************************************************************/
void dvmrp_neighbor_cleanup (dvmrp_neighbor_t *nbr);

#endif
