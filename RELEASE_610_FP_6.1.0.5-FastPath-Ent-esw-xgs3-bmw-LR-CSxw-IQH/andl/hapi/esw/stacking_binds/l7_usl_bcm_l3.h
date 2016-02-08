/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm_l3.h
*
* @purpose    Handle synchronization responsibilities for Routing and ARP tables
*
* @component  HAPI
*
* @comments   none
*
* @create     11/19/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/

#ifndef L7_USL_BCM_L3_H
#define L7_USL_BCM_L3_H

#include "l7_common.h"

#ifdef L7_ROUTING_PACKAGE 

#include "osapi.h"
#include "l7_usl_bcm.h"
#include "bcmx/l3.h"
#include "bcmx/tunnel.h"

/*********************************************************************
* @purpose  Initialize the IPMC tables, semaphores, ...
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized 
*
* @notes    On error, all resources will be released
*       
* @end
*********************************************************************/
extern L7_RC_t usl_l3_init();
 
/*********************************************************************
* @purpose  Deallocate the IPMC tables, semaphores, ...
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized 
*       
* @end
*********************************************************************/
extern L7_RC_t usl_l3_fini();

/*********************************************************************
* @purpose  Create an L3 Interface
*
* @param    intfInfo      @{(input)} Interface attributes
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_l3_intf_create(bcmx_l3_intf_t *info);

/*********************************************************************
* @purpose  Delete an L3 Interface
*
* @param    intfInfo      @{(input)} Interface attributes
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_l3_intf_delete(bcmx_l3_intf_t *info);

/*********************************************************************
* @purpose  Get the max route entries that can be handled  in a single 
*           transaction
*
* @param    none
*
* @returns  Number of route entries in a bulk transaction
*
* @end
*********************************************************************/
L7_uint32 usl_l3_route_max_entries_get(void);

/*********************************************************************
* @purpose  Add route entry (or entries) to the table
*
* @param    info     @{(input)}  The address, masks, and associated data
* @param    count    @{(input)}  Number of route entries passed
* @param    *rv      @{(output)} Return code each of the routes (SDK rv)
*
* @returns  L7_RC_t
*
* @notes    Caller must allocate enough memory for output param
*
* @end
*********************************************************************/
L7_RC_t usl_bcmx_l3_route_add(bcmx_l3_route_t *info, L7_uint32 count, L7_int32 *rv);


/*********************************************************************
* @purpose  Delete an LPM entry or entries from the table
*
* @param    info     @{(input)}  The address, masks, and associated data
* @param    count    @{(input)}  Number of route entries passed
* @param    *rv      @{(output)} Return code each of the routes (SDK rv)
*
* @returns  L7_RC_t
*
* @notes    Caller must allocate enough memory for output param
*
* @end
*********************************************************************/
L7_RC_t usl_bcmx_l3_route_delete(bcmx_l3_route_t *info, L7_uint32 count, L7_int32 *rv);

/*********************************************************************
* @purpose  Get the max host entries that can be handled by underlying RPC layer
*           in a single transaction
*
* @param    none
*
* @returns  Number of host entries in a bulk transaction
*
* @end
*********************************************************************/
L7_uint32 usl_l3_host_max_entries_get(void);

/*********************************************************************
* @purpose  Add a Host entry(or entries)
*
* @param    info     @{(input)}  The address and associated data
* @param    count    @{(input)}  Number of host entries passed
* @param    *rv      @{(output)} Return code each of the hosts (SDK rv)
*
* @returns  L7_RC_t
*
* @notes    Caller must allocate enough memory for output param
*
* @end
*********************************************************************/
L7_RC_t usl_bcmx_l3_host_add(bcmx_l3_host_t *info, L7_uint32 count, L7_int32 *rv);


/*********************************************************************
* @purpose  Delete a host entry or entries from the table
*
* @param    info     @{(input)}  IP address and associated data
* @param    count    @{(input)}  Number of host entries passed
* @param    *rv      @{(output)} Return code each of the host (SDK rv)
*
* @returns  L7_RC_t
*
* @notes    Caller must allocate enough memory for output param
*
* @end
*********************************************************************/
L7_RC_t usl_bcmx_l3_host_delete(bcmx_l3_host_t *info, L7_uint32 count, L7_int32 *rv);


/*********************************************************************
* @purpose  Get the max egress nhop entries that can be handled 
*           in a single transaction
*
* @param    none
*
* @returns  Number of egress nhop entries in a bulk transaction
*
* @end
*********************************************************************/
L7_uint32 usl_l3_egress_nhop_max_entries_get(void);


/*********************************************************************
* @purpose  Create an egress next hop object(s)
*
* @param    pFlags    @{(input)}  Flags for each nexthops
* @param    pBcmxInfo @{(input)}  BCMX data for each nexthops
* @param    count     @{(input)}  Number of nexthops passed
* @param    pEgrIntf  @{(output)} Egress Ids of the nexthops
* @param    rv        @{(output)} Return code each of the routes (SDK rv)
*
* @returns  L7_RC_t
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usl_bcmx_l3_egress_create(L7_uint32 *pFlags,
                                  bcmx_l3_egress_t *pBcmxInfo,
                                  L7_uint32 count,
                                  bcm_if_t *pEgrIntf,
                                  L7_int32 *rv);

/*********************************************************************
* @purpose  Destroy an egress next hop object(s)
*
* @param    pEgrIntf    @{(input)} Egress object interface id(s)
* @param    count       @{(input)} Number of egress Ids to destroy
* @param    rv          @{(output)} Return code for each entry
*
* @returns  L7_RC_t
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usl_bcmx_l3_egress_destroy(bcm_if_t *pEgrIntf, L7_uint32 count,
                                   L7_int32 *rv);


/*********************************************************************
* @purpose  Create a multipath/ECMP egress next hop object
*
* @param    flags        @{(input)} Flags = create/replace
* @param    intf_count   @{(input)} Number of egress nhops
* @param    intf_array   @{(input)} Array of egress nhop Ids
* @param    mpintf       @{(input/ouput)} Multipath egress obj id
*
* @returns  Defined by the Broadcom driver
*
* @notes
*
* @end
*********************************************************************/
int usl_bcmx_l3_egress_multipath_create(L7_uint32 flags, L7_int32 intf_count,
                                        bcm_if_t * intf_array, bcm_if_t *mpintf);

/*********************************************************************
* @purpose  Destroy a multipath/ECMP egress next hop object
*
* @param    mpintf   @{(input)} Multipath egress object id
*
* @returns  Defined by the Broadcom driver
*
* @notes
*
* @end
*********************************************************************/
int usl_bcmx_l3_egress_multipath_destroy(bcm_if_t mpintf);


/*********************************************************************
* @purpose  Clear all entries from the L3 databases
*
* @param    void
*
* @returns  L7_SUCCESS or L7_ERROR 
*
* @notes    Use this function to reset all the tables to the default, no 
*           resources will be deallocated
*
* @end
*********************************************************************/
extern L7_RC_t usl_l3_database_invalidate(void);

/*********************************************************************
* @purpose  Add a Tunnel Initiator entry to the table
*
* @param    intf       @{(input)} The tunnel interface info
* @param    initiator  @{(input)} The tunnel initiator info
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_tunnel_initiator_set(bcmx_l3_intf_t * intf,
                                     bcmx_tunnel_initiator_t * initiator);

/*********************************************************************
* @purpose  Delete a Tunnel Initiator entry from the table
*
* @param    intf       @{(input)} The tunnel interface info
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_tunnel_initiator_clear(bcmx_l3_intf_t * intf);

/*********************************************************************
* @purpose  Add a Tunnel Terminator entry to the table
*
* @param    terminator @{(input)} The tunnel terminator info
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_tunnel_terminator_add(
                                bcmx_tunnel_terminator_t * terminator);

/*********************************************************************
* @purpose  Delete a Tunnel Terminator entry from the table
*
* @param    terminator @{(input)} The tunnel terminator info
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_tunnel_terminator_delete(
                                  bcmx_tunnel_terminator_t * terminator);

/*********************************************************************
* @purpose  Add a WLAN virtual port to an IP multicast group
*
* @param    unit       @{(input)} bcm unit
* @param    index      @{(input)} ipmc hardware table index
* @param    vlan_id    @{(input)} vlan routing interface id
* @param    port       @{(input)} wlan gport
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_l3_ipmc_wlan_port_add(L7_int32 unit, L7_int32 index, 
                                  L7_uint32 vlan_id, bcm_port_t port);

/*********************************************************************
* @purpose  Delete a WLAN virtual port from an IP multicast group
*
* @param    unit       @{(input)} bcm unit
* @param    index      @{(input)} ipmc hardware table index
* @param    vlan_id    @{(input)} vlan routing interface id
* @param    port       @{(input)} wlan gport
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_l3_ipmc_wlan_port_delete(L7_int32 unit, L7_int32 index, 
                                     L7_uint32 vlan_id, bcm_port_t port);

/*********************************************************************
* @purpose  Add a WLAN virtual port to an IP multicast group (L2 bitmap)
*
* @param    unit       @{(input)} bcm unit
* @param    index      @{(input)} ipmc hardware table index
* @param    port       @{(input)} wlan gport
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_l2_ipmc_wlan_port_add(L7_int32 unit, L7_int32 index, 
                                  bcm_port_t port);

/*********************************************************************
* @purpose  Delete a WLAN virtual port from an IP multicast group (L2 bitmap)
*
* @param    unit       @{(input)} bcm unit
* @param    index      @{(input)} ipmc hardware table index
* @param    port       @{(input)} wlan gport
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_l2_ipmc_wlan_port_delete(L7_int32 unit, L7_int32 index, 
                                     bcm_port_t port);

#endif /* L7_ROUTING_PACKAGE */
#endif
