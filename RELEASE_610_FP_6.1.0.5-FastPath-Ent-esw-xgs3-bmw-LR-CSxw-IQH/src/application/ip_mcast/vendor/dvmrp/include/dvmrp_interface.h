/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_interface.h
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
#ifndef _DVMRP_INTERFACE_H
#define _DVMRP_INTERFACE_H
#include <dvmrp_common.h>

/* periodic neighbor probe interval */
#define DVMRP_NEIGHBOR_PROBE_INTERVAL  10      
#define DVMRP_RT_POISON  0x04




typedef struct dvmrp_src_info_s 
{
  L7_inet_addr_t source;                             /*  Source Address   */
  L7_uchar8 interface_mask[L7_INTF_INDICES];  /* list of interfaces on which receivers 
                                     are there*/
}dvmrp_src_info_t;

/*********************************************************************
* @purpose  This function is used to apply the policy on the incoming 
*           interface. 
*
* @param    inetAddr   - @b{(input)}  Pointer to the Network address.
* @param    metric      - @b{(input)}  Cost of the metric
* @param    interface  - @b{(input)} Pointer to the interface
* 
* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
L7_int32 dvmrp_policy_in (L7_inet_addr_t * inetAddr, 
                          L7_int32 metric, 
                          dvmrp_interface_t *interface);

/*********************************************************************
* @purpose  This function is used to apply the policy on the outgoing 
*           interface. 
*
* @param    inetAddr  -  @b{(input)} Pointer to the  Network address.
* @param    metric     -  @b{(input)}   Cost of the metric
* @param    interface  -  @b{(input)}  Pointer to the Incoming interface
* @param    out          -  @b{(input)}   Pointer to the Out going interface
* @param    route       -  @b{(input)}  Pointer to the Route corresponding to this network
*
* 
* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
L7_int32 dvmrp_policy_out (L7_inet_addr_t * inetAddr, 
                           L7_int32 metric, 
                           dvmrp_interface_t *interface,
                           dvmrp_interface_t *out, 
                           dvmrp_route_t * route);



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
* @purpose  This function is used to know whether any other dependent 
*           neighbours on this interface
*
* @param    bitset    -   @b{(input)} neighbour bit set
* @param    interface -   @b{(input)}Pointer to the interface for which the dependent 
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
* @purpose  This function is used to Enable/Disable DVMRP  a given interface
*
* @param    dvmrpcb  -  @b{(input)} Pointer to the  DVMRP control block
* @param    index    -  @b{(input)}  on which the protocol needs to be enabled
* @param    on       -  @b{(input)}  status of the interface-ENABLE or DISABLE
* @param    ipAddr   -  @b{(input)}  Ip address of the interface
* @param    netMask  -  @b{(input)}  ip Mask of the interface
* 
* @returns  None 
*
* @notes    None
*       
* @end
*********************************************************************/

L7_RC_t dvmrp_interface_activate (dvmrp_t *dvmrpcb, 
                                         L7_int32 index, 
                                         L7_int32 on, 
                                         L7_inet_addr_t *ipAddr, 
                                         L7_inet_addr_t *ipMask);


#endif
