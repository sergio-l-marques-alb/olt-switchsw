/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_l3_db.h
*
* @purpose    L3 Db implementation
*
* @component  HAPI
*
* @comments   none
*
* @create     12/5/2008
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#ifndef L7_USL_L3_DB_H
#define L7_USL_L3_DB_H

#include "l7_common.h"

#ifdef L7_ROUTING_PACKAGE

#include "osapi.h"
#include "osapi_support.h"
#include "avl_api.h"
#include "broad_common.h"
#include "l7_usl_bcm.h"
#include "l7_usl_bcm_l3.h"
#include "ptin_globaldefs.h"

typedef struct 
{
  usl_bcm_l3_route_t bcm_data;
  L7_BOOL isValid; /* can be used to traverse the table directly */
  void *avlData;

} usl_lpm_db_elem_t;


/* structure used in the host entry in use messages */
typedef struct
{
  L7_uint32 entryFlags;
  union
  {
    bcm_ip_t v4;
    bcm_ip6_t v6;
  } addr;
} usl_host_inuse_t;

#define L7_USL_HOST_INUSE_FLAG_IPV4   0x40000000
#define L7_USL_HOST_INUSE_FLAG_IPV6   0x60000000
#define L7_USL_HOST_INUSE_FLAG_IPVER  0xF0000000
#define L7_USL_HOST_INUSE_FLAG_IN_SW  0x00000001
#define L7_USL_HOST_INUSE_FLAG_IN_HW  0x00000002
#define L7_USL_HOST_INUSE_FLAG_IN     (L7_USL_HOST_INUSE_FLAG_IN_SW | L7_USL_HOST_INUSE_FLAG_IN_HW)
#define L7_USL_HOST_INUSE_FLAG_ERR_SW 0x00000004
#define L7_USL_HOST_INUSE_FLAG_ERR_HW 0x00000008
#define L7_USL_HOST_INUSE_FLAG_ERR    (L7_USL_HOST_INUSE_FLAG_ERR_SW | L7_USL_HOST_INUSE_FLAG_ERR_HW)
#define L7_USL_HOST_INUSE_FLAG_S_HIT  0x00000010
#define L7_USL_HOST_INUSE_FLAG_D_HIT  0x00000020
#define L7_USL_HOST_INUSE_FLAG_HIT    (L7_USL_HOST_INUSE_FLAG_S_HIT | L7_USL_HOST_INUSE_FLAG_D_HIT)

typedef struct
{
  usl_bcm_l3_host_t bcm_data;
  L7_BOOL          isValid; /* can be used to traverse the table directly */
  L7_uint32        lastHitSrc; /* time since boot of last detected source hit */
  L7_uint32        lastHitDst; /* time since boot of last detected destination hit */
  void *avlData;

} usl_l3_host_db_elem_t;


/* Egress object info for non-ECMP next hop */
typedef struct
{
  usl_bcm_l3_egress_t egrInfo;
  bcm_if_t            egrId; 
  void *avlData;
} usl_egr_nhop_db_elem_t;

/* Egress object info for ECMP group of next hops (multipath) */
typedef struct
{
  bcm_if_t avlKey;
  bcm_if_t intf[L7_RT_MAX_EQUAL_COST_ROUTES];
  L7_uint32 intfCount;
  void *avlData;
} usl_mpath_egr_nhop_db_elem_t;

typedef struct
{
  usl_bcm_l3_intf_t intfInfo;

  L7_BOOL isValid; /* can be used to traverse the table directly */
} usl_l3_intf_db_elem_t;

typedef struct
{
  bcm_if_t intf_id;
} usl_l3_tunnel_initiator_db_key_t;

typedef struct
{
  usl_l3_tunnel_initiator_db_key_t avlKey;
  struct {
    bcm_l3_intf_t intf;
    bcm_tunnel_initiator_t initiator;
  } bcm_data;
  L7_BOOL isValid;
  void *avlData;
} usl_l3_tunnel_initiator_db_elem_t;

typedef struct
{
  bcm_ip_t sip;
  bcm_ip_t dip;
} usl_l3_tunnel_terminator_db_key_t;

typedef struct
{
  usl_l3_tunnel_terminator_db_key_t avlKey;
  bcm_tunnel_terminator_t bcm_data;
  L7_BOOL isValid;
  void *avlData;
} usl_l3_tunnel_terminator_db_elem_t;


typedef struct
{
  L7_BOOL used;
} usl_l3_intf_hw_id_list_t;

typedef struct
{
  L7_BOOL used;
} usl_l3_egr_nhop_hw_id_list_t;

typedef struct
{
  L7_BOOL used;
} usl_l3_mpath_egr_nhop_hw_id_list_t;

#define USL_L3_NTOP(f,a,s,l)    ((L7_uchar8 *)osapiInetNtop((L7_uint32)(f),  \
                                                            (L7_uchar8 *)(a),\
                                                            (L7_uchar8 *)(s),\
                                                            (L7_uint32)(l)))

#define USL_L3_INTF_DB_LEN (platIntfVlanIntfMaxCountGet() + platIntfRouterMaxCountGet() + (PTIN_SYSTEM_N_IGMP_INSTANCES * PTIN_SYSTEM_N_UPLINK_INTERF))

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
* @purpose  Clear all entries from the L3 databases
*
* @param    Database types to invalidate
*
* @returns  L7_SUCCESS or L7_ERROR 
*
* @notes    Use this function to reset all the tables to the default, no 
*           resources will be deallocated
*
* @end
*********************************************************************/
extern L7_RC_t usl_l3_database_invalidate(USL_DB_TYPE_t flags);


/*********************************************************************
* @purpose  Check for in use hosts; update our table & send messages
*           about such hosts to the manager (if we're not manager).
*           More than one message is automatically sent if the table
*           contains more hosts that have been active than will fit
*           into a single message.
*
* @notes    Currently, this does the whole table in a single burst.
*           It would be possible to spread this out over time by
*           adding a last address handled somewhere (maybe a static
*           or perhaps a pointer to one as a parameter) that could
*           be used for resuming, some logic to prevent table wrap
*           until proper time had passed, and more frequent calls
*           to the timer handler so that the entire table would be
*           processed in the expected time frame.
* @end
*********************************************************************/
extern void usl_l3host_active_check(void);

/*********************************************************************
* @purpose  Process the host in use messages from stack members,
*           on the manager.  If, for some reason, a host is reported
*           as in use, but is not in the manager's table, it is
*           quietly ignored, on the grounds that is was probably
*           recently deleted for some reason.
*
* @param    msg     @{(input)} The message with the hosts
*
* @end
*********************************************************************/
extern void usl_l3host_active_process(const L7_uchar8 *msg);

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
*
* @returns  L7_TRUE if it found the host & filled in the flags
*           L7_FALSE if it did not find the host
* 
* @notes    Does nothing but return L7_FALSE if it can't find the host.
*           Copies l3a_flags from Broadcom host data.
*           Clears the USL copy of the _HIT flags.
*           Only fills in last hit times if appropriate _HIT flag was set.
*
* @end
*********************************************************************/
extern L7_BOOL usl_l3host_hit_flags_get(const L7_uint32 family,
                                        const L7_uint8  *l3_addr,
                                        L7_uint32 *flagsPtr,
                                        L7_uint32 *dstTimePtr,
                                        L7_uint32 *srcTimePtr);

/*********************************************************************
* @purpose  Add an L3 interface in USL Db
*
* @param    dbType {(input)} db type to be modified
* @param    info   {(input)} L3 interface info
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_intf_create(USL_DB_TYPE_t dbType,
                          usl_bcm_l3_intf_t *info);

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
int usl_db_l3_intf_get(USL_DB_TYPE_t dbType, usl_bcm_l3_intf_t *info);

/*********************************************************************
* @purpose  Delete an L3 interface from USL Db
*
* @param    dbType {(input)} db type to be modified
* @param    info   {(input)} L3 interface info
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_intf_delete(USL_DB_TYPE_t dbType,
                          usl_bcm_l3_intf_t *info);

/*********************************************************************
* @purpose  Create an Egress Nhop object in USL Db
*
* @param    dbType   @{(input)}  dbType to be modified
* @param    egrIntf  @{(input)}  Egress Id of the nexthop
* @param    pBcmInfo @{(input)}  BCM data for the nexthop
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_egress_create(USL_DB_TYPE_t dbType, bcm_if_t egrIntf, 
                            usl_bcm_l3_egress_t *pBcmInfo);

/*********************************************************************
* @purpose  Destroy an Egress Nhop object in USL Db
*
* @param    dbType   @{(input)}  dbType to be modified
* @param    pBcmInfo  @{(output)} BCM data for the nexthop
* @param    egrIntf  @{(output)} Egress Id of the nexthop
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_egress_destroy(USL_DB_TYPE_t dbType, 
                             usl_bcm_l3_egress_t *pBcmInfo,
                             bcm_if_t egrIntf);

/*********************************************************************
* @purpose  Create a Multipath Egress Nhop object in USL Db
*
* @param    dbType       @{(input)} dbType to be modified
* @param    mpintf       @{(input)} Multipath egress obj id
* @param    intf_count   @{(input)} Number of egress nhops
* @param    intf_array   @{(input)} Array of egress nhop Ids
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_egress_multipath_create(USL_DB_TYPE_t dbType, bcm_if_t mpintf, 
                                      L7_int32 intf_count,
                                      bcm_if_t * intf_array);

/*********************************************************************
* @purpose  Destroy a Multipath Egress Nhop object in USL Db
*
* @param    dbType       @{(input)} dbType to be modified
* @param    mpintf       @{(input)} Multipath egress obj id
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_egress_multipath_destroy(USL_DB_TYPE_t dbType, bcm_if_t mpintf);

/*********************************************************************
* @purpose  Add a host entry in USL Db
*
* @param    dbType {(input)} db type to modify
* @param    info   {(input)} Host info
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_host_add(USL_DB_TYPE_t dbType, usl_bcm_l3_host_t *info);

/*********************************************************************
* @purpose  Delete a host entry from USL Db
*
* @param    dbType {(input)} db type to modify
* @param    info   {(input)} Host info
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_host_delete(USL_DB_TYPE_t dbType, usl_bcm_l3_host_t *info);

/*********************************************************************
* @purpose  Add a route entry in USL Db
*
* @param    dbType {(input)} Db type to modify
* @param    info   {(input)} Route info
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_route_add(USL_DB_TYPE_t dbType, usl_bcm_l3_route_t *info);

/* @purpose  Delete a route entry from USL Db
*
* @param    dbType {(input)} Db type to modify
* @param    info   {(input)} Route info
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_route_delete(USL_DB_TYPE_t dbType, usl_bcm_l3_route_t *info);

/*********************************************************************
* @purpose  Add a tunnel initiator in USL Db
*
* @param    dbTyoe        @{(input)} db type to modify
* @param    intf          @{(input)} The tunnel interface
* @param    initiator     @{(input)} The tunnel initiator info
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_tunnel_initiator_set(USL_DB_TYPE_t dbType,
                                   bcm_l3_intf_t *intf,
                                   bcm_tunnel_initiator_t *initiator);

/*********************************************************************
* @purpose  Delete a tunnel initiator from USL Db
*
* @param    dbType        @{(input)} db type to update
* @param    intf          @{(input)} The tunnel interface
* @param    initiator     @{(input)} The tunnel initiator info
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_tunnel_initiator_clear(USL_DB_TYPE_t dbType,
                                     bcm_l3_intf_t *intf);

/*********************************************************************
* @purpose  Add a tunnel terminator in USL Db
*
* @param    dbType         @{(input)} dbType to be modified
* @param    terminator     @{(input)} The tunnel terminator info
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_tunnel_terminator_add(USL_DB_TYPE_t dbType,
                                    bcm_tunnel_terminator_t *terminator);

/*********************************************************************
* @purpose  Delete a tunnel terminator from USL Db
*
* @param    dbType         @{(input)} dbType to be modified
* @param    terminator     @{(input)} The tunnel terminator info
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l3_tunnel_terminator_delete(USL_DB_TYPE_t dbType,
                                       bcm_tunnel_terminator_t *terminator);

/*********************************************************************
* @purpose  Allocate a hw L3 Intf for a given Interface
*
* @param    intfInfo  @{(input)}  L3 Intf info
* @param    index     @{(output)} Hw Index
*
* @returns  BCM_E_NONE: HwId Generated successfully
*           BCM_E_FULL: No valid HwId exists
*
* @end
*********************************************************************/
int usl_l3_intf_hw_id_allocate(usl_bcm_l3_intf_t *intfInfo, L7_int32 *index);

/*********************************************************************
* @purpose  Free a given L3 Intf index
*
* @param    index   @{(input)} L3 Intf index
*
* @returns  BCM_E_NONE: HwId freed successfully
*           BCM_E_FAIL: HwId could not be freed
*
* @end
*********************************************************************/
int usl_l3_intf_hw_id_free(L7_int32 index);

/*********************************************************************
* @purpose  Allocate a hw Egr Nhop for a given Interface
*
* @param    intfInfo  @{(input)}  L3 Intf info
* @param    index     @{(output)} Hw Index
*
* @returns  BCM_E_NONE: HwId Generated successfully
*           BCM_E_FULL: No valid HwId exists
*
* @end
*********************************************************************/
int usl_l3_egr_nhop_hw_id_allocate(usl_bcm_l3_egress_t *egressInfo, 
                                   bcm_if_t *index);

/*********************************************************************
* @purpose  Reserve/Unreserve a given L3 Egr Nhop index
*
* @param    index    @{(input/output)} L3 Intf index
* @param    reserve  @{input)} L7_TRUE: Reserve the id
*                              L7_FALSE: Unreserve the id
*
* @returns  BCM_E_NONE: HwId was reserved/unreserved successfully
*           BCM_E_FAIL: HwId could not be reserved
*
* @end
*********************************************************************/
int usl_l3_egr_nhop_hw_id_reserve(bcm_if_t *index, L7_BOOL reserve);

/*********************************************************************
* @purpose  Free a given L3 Egr Nhop index
*
* @param    index   @{(input)} L3 Intf index
*
* @returns  BCM_E_NONE: HwId freed successfully
*           BCM_E_FAIL: HwId could not be freed
*
* @end
*********************************************************************/
int usl_l3_egr_nhop_hw_id_free(bcm_if_t index);

/*********************************************************************
* @purpose  Allocate a hw Mpath Egr Nhop 
*
* @param    intf_count   @{(input)} Number of egress nhops
* @param    intf_array  @{(input)} List of egress nhops 
* @param    index      @{(output)} Hw Index
*
* @returns  BCM_E_NONE: HwId Generated successfully
*           BCM_E_FULL: No valid HwId exists
*
* @end
*********************************************************************/
int usl_l3_mpath_egr_nhop_hw_id_allocate(L7_uint32 intf_count, 
                                         bcm_if_t *intf_array, 
                                         bcm_if_t *index);

/*********************************************************************
* @purpose  Free a given L3 Mpath Egr Nhop index
*
* @param    index   @{(input)} Mpath Intf index
*
* @returns  BCM_E_NONE: HwId freed successfully
*           BCM_E_FAIL: HwId could not be freed
*
* @end
*********************************************************************/
int usl_l3_mpath_egr_nhop_hw_id_free(bcm_if_t index);


#ifdef L7_STACKING_PACKAGE
/*********************************************************************
* @purpose  Used to clean up the HW L3 tables of any references to 
*           modids of the old manager unit. This function is called
*           by each stack unit after a failover occurs.
*           This function also updates nexthop entries that point
*           to the CPU so that they point to the CPU port of the 
*           new manager.
*
* @param    *missing_mod_ids      @{(input)} array of missing modIds
*            missing_mod_id_count @{(input)} count of missing modIds
*            manager_left_stack   @{(input)} indicates if this function
*                                            is called as a result of
*                                            the manager leaving the stack.
*            old_cpu_modid        @{(input)} modid of old CPU port.
*            old_cpu_modport      @{(input)} modport of old CPU. 
*            new_cpu_modid        @{(input)} modid of new CPU port.
*            new_cpu_modport      @{(input)} modport of new CPU. 
*
* @returns  BCM_E_NONE
*           BCM_E_FAIL
*
* @end
*********************************************************************/
int usl_l3_db_dataplane_cleanup(L7_int32 *missing_mod_ids, 
                                L7_uint32 missing_mod_id_count,
                                L7_BOOL   manager_left_stack,
                                L7_uint32 old_cpu_modid, 
                                L7_uint32 old_cpu_modport,
                                L7_uint32 new_cpu_modid, 
                                L7_uint32 new_cpu_modport);
#endif

#endif /* L7_ROUTING_PACKAGE */

#endif
