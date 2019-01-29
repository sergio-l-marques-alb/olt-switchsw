#ifndef L7_RPC_L3_H_INCLUDED
#define L7_RPC_L3_H_INCLUDED
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm_l3.h
*
* @purpose    New layer to handle directing driver calls to specified chips
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

#include "l7_common.h"

#ifdef L7_ROUTING_PACKAGE

#include "osapi.h"
#include "l7_usl_bcm_l3.h"

/*********************************************************************
* @purpose  Create an L3 Interface
*
* @param    info     @{(input)} Interface attributes
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int l7_rpc_client_l3_intf_create(usl_bcm_l3_intf_t *info,
                                        L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Delete an L3 Interface
*
* @param    info     @{(input)} Interface attributes
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int l7_rpc_client_l3_intf_delete(usl_bcm_l3_intf_t *info,
                                        L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Get the max egress objects that can be handled in a RPC transaction
*
* @param    none
*
* @returns  Number of egress objects per RPC transaction
*
* @end
*********************************************************************/
L7_uint32 l7_custom_rpc_l3_max_egress_nhop_get(void);


/*********************************************************************
* @purpose  Create a L3 egress next hop object(s)
*
* @param    flags       @{(input)} Egress object flags
* @param    pBcmxInfo   @{(input)} Egress object attributes
* @param    count       @{(input)} Number of egress objects
* @param    egrIntf     @{(input/output)} Egress object id(s)
* @param    rv          @{(output)} Return code(s)
* @param    numChips    @{(input)} The number of bcm unit IDs in the list
* @param    chips       @{(input)} The list of bcm unit IDs
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_client_l3_egress_create(L7_uint32 *pFlags, usl_bcm_l3_egress_t *pBcmxInfo,
                                       L7_uint32 count, bcm_if_t *pEgrIntf, int *rv,
                                        L7_uint32 numChips, L7_uint32 *chips);


/*********************************************************************
* @purpose  Destroy an L3 egress next hop object(s)
*
* @param    pBcmInfo @{(input)}  BCMX data for each nexthops
* @param    pEgrIntf @{(input)} Egress object id(s)
* @param    count    @{(input)  Number of egress objects to delete
* @param    rv       @{(output) Return code for each delete
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  L7_RC_t  L7_SUCCESS if transaction completed and rv is valid
*                    L7_FAILURE if transaction failed, output param is invalid
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_client_l3_egress_destroy(usl_bcm_l3_egress_t *pBcmInfo,
                                        bcm_if_t *pEgrIntf, L7_uint32 count, 
                                        L7_int32 *rv, L7_uint32 numChips, 
                                        L7_uint32 *chips);


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
int l7_rpc_client_l3_egress_multipath_create(L7_uint32 flags, L7_int32 intf_count,
                                             bcm_if_t * intf_array, bcm_if_t *mpintf,
                                             L7_uint32 numChips, L7_uint32 *chips);

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
int l7_rpc_client_l3_egress_multipath_destroy(bcm_if_t mpintf,
                                              L7_uint32 numChips, L7_uint32 *chips);


/*********************************************************************
* @purpose  Get the max host entries that can be handled in a RPC transaction
*
* @param    none
*
* @returns  Number of host entries per RPC transaction
*
* @end
*********************************************************************/
L7_uint32 l7_custom_rpc_l3_max_host_get(void);


/*********************************************************************
* @purpose  Add a Host entry or entries to the table
*
* @param    info     @{(input)} The address and associated data
* @param    count    @{(input)}  Number of host entries passed
* @param    *rv      @{(output)} Return code each of the hosts (SDK rv)
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  L7_RC_t  L7_SUCCESS if transaction completed and rv is valid
*                    L7_FAILURE if transaction failed, output param is invalid
*
* @notes    It is expected that the caller should only send data that
*           can fit in a single RPC transaction. In other words, there is
*           an upper limit on the number of hosts that can be sent in an
*           RPC transaction. See l7_custom_rpc_l3_max_host_get().
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_client_l3_host_add(usl_bcm_l3_host_t * info, 
                                  L7_uint32 count, L7_int32 *rv,
                                  L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Delete a Host entry or entries
*
* @param    host     @{(input)} The host to remove
* @param    count    @{(input)}  Number of host entries passed
* @param    *rv      @{(output)} Return code each of the hosts (SDK rv)
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  L7_RC_t  L7_SUCCESS if transaction completed and rv is valid
*                    L7_FAILURE if transaction failed, output param is invalid
*
* @notes    It is expected that the caller should only send data that
*           can fit in a single RPC transaction. In other words, there is
*           an upper limit on the number of hosts that can be sent in an
*           RPC transaction. See l7_custom_rpc_l3_max_host_get().
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_client_l3_host_delete(usl_bcm_l3_host_t * info,
                                     L7_uint32 count, L7_int32 *rv,
                                     L7_uint32 numChips, L7_uint32 *chips);


/*********************************************************************
* @purpose  Add an LPM entry or entries to the table
*
* @param    info     @{(input)} The address, masks, and associated data
* @param    count    @{(input)}  Number of route entries passed
* @param    *rv      @{(output)} Return code each of the routes (SDK rv)
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  L7_RC_t  L7_SUCCESS if transaction completed and rv is valid
*                    L7_FAILURE if transaction failed, output param is invalid
*
* @notes    It is expected that the caller should only send data that
*           can fit in a single RPC transaction. In other words, there is
*           an upper limit on the number of routes that can be sent in an
*           RPC transaction. See l7_custom_rpc_l3_max_route_get().
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_client_l3_route_add(usl_bcm_l3_route_t * info,
                                   L7_uint32 count, L7_int32 *rv,
                                   L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Delete LPM entry or entries from the table
*
* @param    info     @{(input)} The address, masks, and associated data
* @param    count    @{(input)}  Number of route entries passed
* @param    *rv      @{(output)} Return code each of the routes (SDK rv)
* @param    numChips @{(input)} The number of bcm unit IDs in the list
* @param    chips    @{(input)} The list of bcm unit IDs
*
* @returns  L7_RC_t  L7_SUCCESS if transaction completed and rv is valid
*                    L7_FAILURE if transaction failed, output param is invalid
*
* @notes    It is expected that the caller should only send data that
*           can fit in a single RPC transaction. In other words, there is
*           an upper limit on the number of routes that can be sent in an
*           RPC transaction. See l7_custom_rpc_l3_max_route_get().
* @end
*********************************************************************/
L7_RC_t l7_rpc_client_l3_route_delete(usl_bcm_l3_route_t * info,
                                      L7_uint32 count, L7_int32 *rv,
                                      L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Get the max route entries that can be handled in a RPC transaction
*
* @param    none
*
* @returns  Number of route entries per RPC transaction
*
* @end
*********************************************************************/
L7_uint32 l7_custom_rpc_l3_max_route_get(void);


/*********************************************************************
* @purpose  Add a Tunnel Initiator entry to the table
*
* @param    intf       @{(input)} Tunnel interface info
* @param    initiator  @{(input)} Tunnel terminator info
* @param    numChips   @{(input)} The number of bcm unit IDs in the list
* @param    chips      @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int l7_rpc_client_tunnel_initiator_set(bcm_l3_intf_t *intf,
                                              bcm_tunnel_initiator_t *initiator,
                                              L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Delete a Tunnel Initiator entry from the table
*
* @param    intf       @{(input)} Tunnel interface info
* @param    numChips   @{(input)} The number of bcm unit IDs in the list
* @param    chips      @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int l7_rpc_client_tunnel_initiator_clear(bcm_l3_intf_t *intf,
                                                L7_uint32 numChips,
                                                L7_uint32 *chips);

/*********************************************************************
* @purpose  Add a Tunnel Terminator entry to the table
*
* @param    terminator @{(input)} Tunnel terminator info
* @param    numChips   @{(input)} The number of bcm unit IDs in the list
* @param    chips      @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int l7_rpc_client_tunnel_terminator_add(
                                    bcm_tunnel_terminator_t *terminator,
                                    L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Delete a Tunnel Terminator entry from the table
*
* @param    terminator @{(input)} Tunnel terminator info
* @param    numChips   @{(input)} The number of bcm unit IDs in the list
* @param    chips      @{(input)} The list of bcm unit IDs
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int l7_rpc_client_tunnel_terminator_delete(
                                     bcm_tunnel_terminator_t *terminator,
                                     L7_uint32 numChips, L7_uint32 *chips);

/*********************************************************************
* @purpose  Convert BCMX terminator structure to the BCM terminator structure
*
* @param    l3xtun     @{(input)} BCMX terminator info
* @param    l3tun      @{(input)} BCM terminator info
*
* @returns  void
*
* @end
*********************************************************************/
extern void l7_bcmx_terminator_to_bcm(bcmx_tunnel_terminator_t *l3xtun,
                                      bcm_tunnel_terminator_t *l3tun);

/*********************************************************************
* @purpose  Initialize custom L3 RPC calls.
*
* @param    none
*
* @returns  none
*
* @end
*********************************************************************/
L7_RC_t l7_rpc_l3_init (void);

#endif /*  L7_ROUTING_PACKAGE */
#endif /*  L7_RPC_L3_H_INCLUDED */
