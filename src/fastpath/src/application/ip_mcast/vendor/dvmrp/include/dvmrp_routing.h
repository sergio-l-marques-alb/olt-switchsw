/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_routing.h
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
#ifndef _DVMRP_ROUTING_H_
#define _DVMRP_ROUTING_H_
#include <dvmrp_common.h>

/* Route learned from the directly connected interface */
#define DVMRP_LOCAL_ROUTE 0x01



/*********************************************************************
* @purpose  This function is used to reset the dependency. 
*
* @param     dvmrpcb -  @b{(input)}Pointer to the DVMRP control block .
* @param     route     -  @b{(input)}Pointer to the route entry for this dependency is removed .
* @param     nbr        -  @b{(input)}Pointer to the nbr node  
*            
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    None
*       
* @end
*********************************************************************/
void dvmrp_dependent_reset(dvmrp_t *dvmrpcb,dvmrp_route_t * route, dvmrp_neighbor_t *nbr);

/*********************************************************************
* @purpose  This function stores routes to be advertised in a linked list. 
*
* @param   all                         -  @b{(input)}   Flag to indicate whether all the 
*                                                            routes to be included or only the 
*                                                              modified ones.
* @param   interface                -  @b{(input)}Pointer to the interface to which routes to be 
*                                                              advertised.
* @param   no_of_rts_advertise -  @b{(output)} no of routes would be stored in this 
*                                                                      list
*                                            
* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
void dvmrp_routes_prepare(L7_int32 all, dvmrp_interface_t *interface,
                          L7_uint32 *no_of_rts_advertise);

/*********************************************************************
* @purpose  This function advertises routes on all interfaces
*
* @param     all            -  @b{(input)}  Flag to indicate whether all the 
*                                            routes to be included or only the 
*                                           modified ones.
* @param     dvmrpcb   -  @b{(input)}Pointer to the DVMRP control block
* 
* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
void dvmrp_route_advertise(L7_int32 all, dvmrp_t *gDvmrp);

/*********************************************************************
* @purpose  This function is called when ever cache table needs to eb updated
*
* @param     code    -  @b{(input)}  type of event.
* @param     entry   -  @b{(input)}Pointer to the cache entry to be updated
* 
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    None
*       
* @end
*********************************************************************/
L7_int32 dvmrp_update_fn_call(L7_int32 code, dvmrp_cache_entry_t *entry, 
                              L7_uint32 ifindex);

/*********************************************************************
* @purpose  This function is called to inject direct routes
* 
* @param     interface  -  @b{(input)}Pointer to the interface for which the direct route
*                                       is being introduced
* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
void dvmrp_direct_route_inject(dvmrp_interface_t *interface);


/*********************************************************************
* @purpose  This function is used to update the route entry. 
*
* @param     inetAddr -  @b{(input)}Pointer to the network address .
*                  metric   -   @b{(input)} -Metric of this netwrok
*                  nbr      -    @b{(input)}Pointer to the nbr node  
*                  now      -   @b{(input)} current time        
*            
* @returns  L7_ERROR/L7_FAILURE/L7_SUCCESS
*
* @notes    None
*       
* @end
*********************************************************************/
L7_int32 dvmrp_route_update (L7_dvmrp_inet_addr_t *inetAddr, L7_int32 metric,
                             dvmrp_neighbor_t *nbr, L7_uint32 now);


/*********************************************************************
* @purpose  This function is used to process the report. 
*
* @param     dvmrpcb          -  @b{(input)}Pointer to the DVMRP control block .
* @param    no_of_reports  -  @b{(input)}  No of Reports
* 
* @returns  L7_FAILURE/L7_SUCCESS
*
* @notes    None
*       
* @end
*********************************************************************/
L7_int32 dvmrp_report_process(dvmrp_t *dvmrpcb, L7_uint32 no_of_reports);

/*********************************************************************
* @purpose  This function is used to create the new route entry. 
*
* @aparm     Proto                 -  @b{(input)}DVMRP protocol identifier
* @param     inetAddr            -  @b{(input)}Pointer to the Network address
* @param     interface           -  @b{(input)}Pointer to the interface through which this route 
*                                                  has been recieved.
* @param     metric               -  @b{(input)}metric of this network
* @param     received_metric -  @b{(input)}- recieved metric
* @param     received_metric -  @b{(input)}- recieved metric
* @param     route                 -  @b{(input)}
* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
void  dvmrp_new_route (L7_int32 proto, 
                       L7_dvmrp_inet_addr_t *inetAddr, 
                       dvmrp_interface_t* interface,
                       dvmrp_neighbor_t *neighbor, 
                       L7_int32 metric, L7_int32 received_metric, dvmrp_route_t  *route );


#endif
