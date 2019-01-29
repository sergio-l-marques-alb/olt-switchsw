/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcmx_l3.h
*
* @purpose    USL BCMX API's 
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

#ifndef L7_USL_BCMX_L3_H
#define L7_USL_BCMX_L3_H

#include "l7_common.h"

#ifdef L7_ROUTING_PACKAGE

#include "l7_usl_bcm_l3.h"

/*********************************************************************
* @purpose  take next hop db sema
*
* @returns  void
*
* @end
*********************************************************************/
extern void usl_bcmx_l3_egr_nhop_take(void);

/*********************************************************************
* @purpose  give next hop db sema
*
* @returns  void
*
* @end
*********************************************************************/
extern void usl_bcmx_l3_egr_nhop_give(void);

/*********************************************************************
* @purpose  Create an L3 Interface
*
* @param    intfInfo      @{(input)} Interface attributes
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_l3_intf_create(usl_bcm_l3_intf_t *info);

/*********************************************************************
* @purpose  Get info for a L3 intf 
*
* @param    info          @{(input)} Interface attributes
*
* @returns  Defined by the Broadcom driver
*
* @notes    L3-id is used to search for the matching intf
*
* @end
*********************************************************************/
extern int usl_bcmx_l3_intf_get(usl_bcm_l3_intf_t *info);


/*********************************************************************
* @purpose  Delete an L3 Interface
*
* @param    intfInfo      @{(input)} Interface attributes
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_l3_intf_delete(usl_bcm_l3_intf_t *info);


/*********************************************************************
* @purpose  Get the max egress nhop entries that can be handled by underlying
*           RPC layer in a single transaction
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
* @param    pBcmInfo @{(input)}  BCMX data for each nexthops
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
                                  usl_bcm_l3_egress_t *pBcmInfo,
                                  L7_uint32 count,
                                  bcm_if_t *pEgrIntf,
                                  L7_int32 *rv);

/*********************************************************************
* @purpose  Destroy an egress next hop object(s)
*
* @param    pBcmInfo @{(input)}  BCMX data for each nexthops
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
L7_RC_t usl_bcmx_l3_egress_destroy(usl_bcm_l3_egress_t *pBcmInfo,
                                   bcm_if_t *pEgrIntf, L7_uint32 count, 
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
* @notes    Forces hit to be true for source and destination in the
*           host data.  Also sets emtry last hit times to current uptime.
*
* @end
*********************************************************************/
L7_RC_t usl_bcmx_l3_host_add(usl_bcm_l3_host_t *info, L7_uint32 count, L7_int32 *rv);


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
L7_RC_t usl_bcmx_l3_host_delete(usl_bcm_l3_host_t *info, L7_uint32 count, L7_int32 *rv);


/*********************************************************************
* @purpose  Get the max route entries that can be handled by underlying RPC layer
*           in a single transaction
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
L7_RC_t usl_bcmx_l3_route_add(usl_bcm_l3_route_t *info, L7_uint32 count, L7_int32 *rv);


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
L7_RC_t usl_bcmx_l3_route_delete(usl_bcm_l3_route_t *info, L7_uint32 count, L7_int32 *rv);


/*********************************************************************
* @purpose  Add a tunnel initiator
*
* @param    intf          @{(input)} The tunnel interface
* @param    initiator     @{(input)} The tunnel initiator info
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_tunnel_initiator_set(bcm_l3_intf_t *intf,
                                         bcm_tunnel_initiator_t *initiator);

/*********************************************************************
* @purpose  Delete a tunnel initiator
*
* @param    intf        @{(input)} The tunnel interface
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_tunnel_initiator_clear(bcm_l3_intf_t *intf);

/*********************************************************************
* @purpose  Add a tunnel terminator
*
* @param    terminator     @{(input)} The tunnel terminator info
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_tunnel_terminator_add(
                                  bcm_tunnel_terminator_t *terminator);

/*********************************************************************
* @purpose  Delete a tunnel terminator
*
* @param    terminator     @{(input)} The tunnel terminator info
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
extern int usl_bcmx_tunnel_terminator_delete(
                                    bcm_tunnel_terminator_t *terminator);

#ifdef L7_STACKING_PACKAGE
/*********************************************************************
* @purpose  Get hit (and some other) flags about a host from the
*           USL host entry for that host, also get the last hit times
*           for that host.
*
* @param    family        @{(input)}  The address family
* @param    l3_addr       @{(input)}  Pointer to the host L3 address
* @param    flagsPtr      @{(output)} Pointer where to put flags
* @param    dstTimePtr    @{(output)} Pointer where to put dest last hit time
* @param    srcTimePtr    @{(output)} Pointer where to put src last hit time
* @param    inHw          @{(output)} L7_TRUE if it found the host & filled in the flags
*                                     L7_FALSE if it did not find the host
*
* @returns  BCM_ERROR_CODE
*
* @notes    
*           Copies l3a_flags from Broadcom host data.
*           Clears the USL copy of the _HIT flags.
*           Only fills in last hit times if appropriate _HIT flag was set.
*
* @end
*********************************************************************/
int usl_bcmx_l3host_hit_flags_get(const L7_uint32 family,
                                  const L7_uint8  *l3_addr,
                                  L7_uint32 *flagsPtr,
                                  L7_uint32 *dstTimePtr,
                                  L7_uint32 *srcTimePtr,
                                  L7_BOOL   *inHw);

#endif

#endif /* L7_ROUTING_PACKAGE */
#endif
