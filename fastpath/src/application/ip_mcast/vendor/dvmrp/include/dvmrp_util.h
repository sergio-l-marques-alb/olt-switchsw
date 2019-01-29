/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dvmrp_util.h
*
* @purpose   This file contains prototypes for operations on the cache table
*
* @component DVMRP CACHE TABLE API's (AVL Trees)
*
* @comments  Access to elements in the list is protected by a semaphore.
*            Functions that return values from the list always return a copy
*            of the data in the list.
*
* @create    10/02/2006
*
* @author    shashidhar Dasoju
*
* @end
*
**********************************************************************/
#ifndef _DVMRP_UTIL_H_
#define _DVMRP_UTIL_H_
#include <dvmrp_common.h>

/*********************************************************************
* @purpose  This function is used to check whether the mask is a valid 
*           mask or not. 
* @param     mask   -  @b{(input)} Given mask .
* 
* @returns  0/1
*
* @notes    None
*       
* @end
*********************************************************************/
L7_int32 inet_mask_valid (L7_uchar8 *mask);


/*********************************************************************
* @purpose  This function is used to delete the route entry.
*
* @param     route   -  @b{(input)}Pointer to the Route entry to be deleted .
*
* @returns  None
*
* @notes    None
*
* @end
*********************************************************************/
void dvmrp_route_del (dvmrp_route_t *route);


/*********************************************************************
*
* @purpose  The delete function is provided to delete both data and
*           node part of the SLL Node.
*
* @param    node -  @b{(input)} The pointer to the node that is to be deleted.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    -   None.
*
* @end
*********************************************************************/
L7_RC_t dvmrp_SLLNodeAnnRt_delete(L7_sll_member_t *node);

/*********************************************************************
* @purpose  This function compares the interfaces. 
*
* @param    node1 -   @b{(input)} Node 1.
* @param    node2 -   @b{(input)} Node 2.
* @param    len   -   @b{(input)} Length of the data.
*
* returns Zero or Non Zero
* 
* @notes    This function uses interface index as 
*           the key for comparision.
*       
* @end
*********************************************************************/
L7_int32 dvmrp_interface_comp (void *node1, void *node2, L7_uint32 len);

/*********************************************************************
* @purpose  This function compares the neighbors. 
*
* @param    node1 -   @b{(input)} Node 1.
* @param    node2 -   @b{(input)} Node 2.
* @param    len   -   @b{(input)} Length of the data.
* 
* @returns  Zero or Non Zero
*
* @notes    This function uses both index and neighbor addresses as
*           the key for comparision.
*       
* @end
*********************************************************************/
L7_int32 dvmrp_neighbor_comp (void *node1, void *node2, L7_uint32 len);


/*********************************************************************
* @purpose  This function compares the prunes. 
*
* @param    node1 -   @b{(input)} Node 1.
* @param    node2 -   @b{(input)} Node 2.
* @param    len   -   @b{(input)} Length of the data.
*
* @returns Zero or Non Zero
*
* @notes    This function uses both expire and neighbor addresses as
*           the key for comparision.
*       
* @end
*********************************************************************/
L7_int32 dvmrp_prune_comp (void *node1, void *node2, L7_uint32 len);

/*********************************************************************
* @purpose  This function compares the grafts. 
*
* @param    node1 -   @b{(input)} Node 1.
* @param    node2 -   @b{(input)} Node 2.
* @param    len   -   @b{(input)} Length of the data.

* @returns  Zero or Non Zero
*
* @notes    This function uses both expire and neighbor addresses as
*           the key for comparision.
*       
* @end
*********************************************************************/
L7_int32 dvmrp_graft_comp (void *node1, void *node2, L7_uint32 len);

/*********************************************************************
* @purpose  This function compares the reports.
*
* @param    node1 -   @b{(input)} Node 1.
* @param    node2 -   @b{(input)} Node 2.
* @param    len   -   @b{(input)} Length of the data.
*
* @returns  Zero or Non Zero
* @notes    This function uses both srcNetwork and neighbor addresses as
*           the key for comparision.
*
* @end
*********************************************************************/
L7_int32 dvmrp_report_comp (void *node1, void *node2, L7_uint32 len);

/*********************************************************************
* @purpose  This function is used to bring the neighbors down.
*
* @param   dvmrpcb - @b{(input)} Pointer to the DVMRP Control block
* @returns  None
*
* @notes    None
*
* @end
*********************************************************************/
void dvmrp_neighbours_down (dvmrp_t *dvmrpcb);

/*********************************************************************
* @purpose  This function is used to shut down DVMRP gracefully
*
* @param     interface -Pointer to the interface on which DVMRP should be shut down
* @returns  None
*
* @notes    None
*
* @end
*********************************************************************/
void dvmrp_graceful_shutdown(dvmrp_interface_t *interface);
/*********************************************************************
* @purpose  This function is used to delete the direct routes
*
* @param     interface -  @b{(input)}Pointer to the Interface for which the direct routes
*                                     should be deleted
* @returns  None
*
* @notes    None
*
* @end
*********************************************************************/
void dvmrp_direct_route_delete(dvmrp_interface_t* interface);

/*********************************************************************
* @purpose  This function is used to get the DF for this route on
*           this interface
*
* @param    route       - @b{(input)}Pointer to the route entry for which the DF is required
* @param    interface   - @b{(input)}Pointer to the  interface on whichthe DF is required
*
* @returns  designated forwarder
*
* @notes    None
*
* @end
*********************************************************************/
desg_fwd_t* dvmrp_get_desg_fwd(dvmrp_route_t *route, 
                               dvmrp_interface_t *interface);

/*********************************************************************
* @purpose  This function is used to increment the count of bad 
*           packets recieved
*
* @param    interface  -  @b{(input)} interface from which the bad packet came
* @param    nbr -        @b{(input)} neighbour from whome the bad packet came
* 
* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
void dvmrp_badpkts_inc(dvmrp_interface_t *interface, dvmrp_neighbor_t *nbr);

/*********************************************************************
* @purpose  This function is used to increment the count of bad routes recieved
*
* @param     interface -  @b{(input)} interface from which the bad route came
* @param     nbr       -  @b{(input)} neighbour from whome the bad route came
* 
* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
void dvmrp_badRts_inc(dvmrp_interface_t *interface, dvmrp_neighbor_t *nbr);

/*********************************************************************
* @purpose  This function is used to check whether prune to be sent or not
*
* @param     dvmrpcb  - @b{(input)} Pointer to the DVMRP control block
* @param     src          - @b{(input)} source address for which the prune to be sent
* @param     grp         -  @b{(input)} group address for which the prune to be sent
* @param     bitlen      -  @b{(input)}-bitlen of the source network
* 
* @returns   1  - if no prune was sent earlier to this source network for 
*                 that group.
*                 0 - if a prune is already sent for that source netowrk for that 
*                 group.
*
* @notes    checks to see if a prune is already sent for that source network 
*           or not.
*            
* @end
*********************************************************************/
L7_int32 dvmrp_send_prune_check(dvmrp_t *dvmrpcb, L7_inet_addr_t *src,L7_inet_addr_t *grp, 
                                L7_uint32 bitlen);



/*********************************************************************
* @purpose  sets up a key for the DVMRP radix tree
*
* @param    p_key - @b{(output)} A pointer to the key
* @param    addr            - @b{(input)} Key value.
*
* @notes    Sets the 1st byte to the length of the key, initializes
*           pad bytes and sets the key bytes to addr.
*
* @end
*********************************************************************/
void dvmrpKeySet(dvmrpTreeKey_t * p_key, L7_inet_addr_t addr);

/*********************************************************************
* @purpose  Determines if the route table is full.
*
* @param    dvmrpcb  - @b{(input)} Pointer to the control block 
* @returns  L7_TRUE   If the table is full
* @returns  L7_FALSE  If the table is not full
*
* @notes    Internal utility function.  The calling function is responsible
*           for acquiring the route table semaphore.
*
* @end
*********************************************************************/
L7_BOOL dvmrpIsFull (dvmrp_t *dvmrpcb);
/*********************************************************************
* @purpose  returns the default mask
*
* @param    family  - @b{(input)} family type 
* @returns  returns default mask
*
* @notes    Internal utility function.  
*
* @end
*********************************************************************/
L7_int32 dvmrpDefaultNetmask (L7_uchar8     family);

/*********************************************************************
*
* @purpose  The delete function is provided to delete both data and
*           node part of the SLL Node.
*
* @param    node -  @b{(input)} The pointer to the node that is to be deleted.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    -   None.
*
* @end
*********************************************************************/
L7_RC_t dvmrp_SLLNodePrune_delete(L7_sll_member_t *node);

/*********************************************************************
* @purpose Boolean OR's the first 2 args, and puts into third arg.
*
* @param    *a   -     @b{(input)}
* @param    *b   -     @b{(input)}
* @param    *c   -     @b{(input)}
* @param    size   -   @b{(input)}
*
* @returns
*
* @notes   None
*
* @end
*********************************************************************/
L7_int32 dvmrpIfor (void *a, void *b, void *c, int size);
/*********************************************************************
* @purpose  Returns whether the content of pointer is Zero or not.
*
* @param    *ptr   -    @b{(input)}  Pointer to Data element.
* @param    size   -    @b{(input)}  Size of the data element.
*
* @returns  0 : if content of data element is non zero
* @returns  1 : if content of data element is zero
*
* @notes   None
*
* @end
*********************************************************************/

L7_int32 dvmrpIfzero (void *ptr, int size);

extern L7_RC_t dvmrpUtilMemoryInit (L7_uint32 addrFamily);

#endif
