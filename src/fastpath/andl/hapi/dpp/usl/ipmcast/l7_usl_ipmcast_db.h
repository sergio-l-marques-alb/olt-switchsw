/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_ipmcast_db.h
*
* @purpose    IP Multicast Db implementation
*
* @component  HAPI
*
* @comments   none
*
* @create     12/2/2008
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#ifndef L7_USL_IPMCAST_DB_H
#define L7_USL_IPMCAST_DB_H

#include "l7_common.h"

#ifdef L7_MCAST_PACKAGE

#include "l7_usl_mask.h"
#include "l7_usl_bcm_ipmcast.h"
#include "l7_usl_bcm_l3.h"


/* AVL Key for IPMC table */
typedef struct
{
  bcm_ip6_t   s_ip_addr;              /* This will hold both ipv4 and ipv6 addresses*/
  bcm_ip6_t   mc_ip_addr;             /* This will hold both ipv4 and ipv6 addresses*/
  bcm_vlan_t  vid;
}usl_ipmc_db_key_t;

/* 
 * Type used to store the MCAST entry
 */
typedef struct
{
  usl_bcm_ipmc_addr_t        bcm_data;
  L7_BOOL                    isValid;  /* can be used when traversing the heap directly */
  L7_uint32                  lastHit;
  void                      *pAvlData;
}usl_ipmc_db_elem_t;

/* 
 * Type used to store the port MCAST entry
 */
typedef struct usl_ipmc_port_db_repl_s
{
  L7_ushort16       vlanId;
  L7_ushort16       ipmcEntryCount;                                                                        /* Count of bits set in ipmcIndexMask. */
  L7_uint32         ipmcIndexMask[(L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL / USL_IPMC_INDEX_MASK_WORD_SIZE) + 1]; /* Indicates which IPMC indices this
                                                                                                              port belongs to for this VLAN ID. */
  struct usl_ipmc_port_db_repl_s *next;
} usl_ipmc_port_db_repl_t;

typedef struct
{
  L7_ushort16                bcmModid;                       /* part of key */
  L7_ushort16                bcmPort;                        /* part of key */
  L7_ushort16                vlanId;
  L7_ushort16                untagFlag;
  L7_uint32                  ipmcIndexMask[(L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL / USL_IPMC_INDEX_MASK_WORD_SIZE) + 1]; /* Indicates which IPMC indices 
                                                                                                                       this port belongs to, 
                                                                                                                       regardless of VLAN ID. */

  usl_ipmc_port_db_repl_t   *replInfo;                       /* Linked list of replication info (usl_ipmc_port_db_repl_t). */
  void                      *pAvlData;
}usl_ipmc_port_db_elem_t;

typedef struct
{
  bcm_if_t         intf;      /* L3 interface (src mac, tunnel)             */
  bcm_vlan_t       vlan;      /* Next hop vlan id.                          */
  bcm_gport_t      lport;     /* Port packet switched to.                   */
  bcm_if_t         egrintf;   /* egress interface interface                 */
  L7_uint32        mc_index;  /* multicast index                            */
  L7_uint32        use_count; /* how many users for this egr interface      */
  L7_uint32        self_owned;/* if owned by some other component, can't delete it */
  usl_bcm_l3_egress_t egr;
  void *avlData;
} usl_ipmc_wlan_port_elem_t;

/*
 * Type used to store inuse entry
 */
typedef struct
{
  bcm_ip6_t  s_ip_addr;   /* This will hold both ipv4 and ipv6 addresses*/
  bcm_ip6_t  mc_ip_addr;  /* This will hold both ipv4 and ipv6 addresses*/
  bcm_vlan_t vid;
  L7_uint32  family;   /* IPv4 or IPv6 */
}l7_usl_ipmc_inuse_t;

typedef struct
{
  L7_BOOL used;
} usl_ipmc_hw_id_list_t;

#define L7_IPV6_ADDRESS       0x0001
#define L7_IPV6_ADDRESS_SIZE  16   /* 16 Bytes */
#define L7_IPV4_ADDRESS_SIZE  4    /* 4 Bytes  */

/*********************************************************************
*
* @purpose Determines if a VLAN vector is empty.
*
* @param   vlanVector {(input)} VLAN replication info for L3 port.
*                     
*
* @returns BCM Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
extern L7_BOOL usl_db_ipmc_vlan_vector_is_empty(bcm_vlan_vector_t vlanVector);

/*********************************************************************
*
* @purpose Sets the VLAN replication configuration for a port in an
*          IPMC entry.
*
* @param   modid      {(input)} modid of L3 port
* @param   bcmPort    {(input)} BCM port
* @param   ipmcEntry  {(input)} IPMC entry
* @param   vlanVector {(input)} VLAN replication info for L3 port.
*
* @returns BCM Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
extern int usl_db_ipmc_repl_set(L7_uint32 modid, L7_uint32 bcmPort, L7_uint32 ipmcIndex, bcm_vlan_vector_t vlanVector);

/*********************************************************************
*
* @purpose Gets the VLAN replication configuration for a port in an
*          IPMC entry.
*
* @param   modid      {(input)}  modid of L3 port
* @param   bcmPort    {(input)}  BCM port
* @param   ipmcEntry  {(input)}  IPMC entry
* @param   vlanVector {(output)} VLAN replication info for L3 port.
*                     
*
* @returns BCM Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
extern int usl_db_ipmc_repl_get(L7_uint32 modid, L7_uint32 bcmPort, L7_uint32 ipmcIndex, bcm_vlan_vector_t vlanVector);

/*********************************************************************
* @purpose  Test whether an IPMC group is in use
*
* @param    bcm_ipmc      @{(input)}  IPMC entry to be used
*
* @returns  L7_TRUE if in use
*
* @notes    DOES NOT ADD DOWNSTREAM PORTS
*
* @end
*********************************************************************/
extern L7_BOOL usl_ipmc_inuse_get(usl_bcm_ipmc_addr_t *bcm_ipmc);

/*********************************************************************
*
* @purpose Sets the IPMC egress config for a port.
*
* @param   modid      {(input)} modid of L3 port
* @param   bcmPort    {(input)} BCM port
* @param   vlanId     {(input)} egress VLAN Id
* @param   untagged   {(input)} egress untagged config
*                     
*
* @returns BCM Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
extern int usl_db_ipmc_egress_port_set(L7_uint32 modid, L7_uint32 bcmPort, L7_ushort16 vlanId, L7_ushort16 untagged);

/*********************************************************************
* @purpose  Check for in use impc groups
*
* @end
*********************************************************************/
extern void usl_ipmc_inuse_check(void);

/*********************************************************************
* @purpose  Process the inuse check message
*
* @param    msg     @{(input)} The message with the groups
*
* @end
*********************************************************************/
extern void usl_ipmc_inuse_process(L7_uchar8 *msg);

/*********************************************************************
* @purpose  Add a IP Mcast entry to the USL Db
*
* @param    bcm_ipmc        {(input)} IP Mcast entry info 
*           replace_entry   {(input)} Indicates if this replaces an existing entry.
*
* @returns BCM Error Code
*
* @end
*********************************************************************/
int usl_db_ipmc_add(usl_bcm_ipmc_addr_t *bcm_ipmc, L7_BOOL replace_entry);

/*********************************************************************
* @purpose  Remove a IPMC group entry
*
* @param    bcm_ipmc {(input)} IP Mcast entry info 
*
* @returns  BCM error code 
*
* @end
*********************************************************************/
int usl_db_ipmc_remove(usl_bcm_ipmc_addr_t *bcm_ipmc);

/*********************************************************************
* @purpose  Update the port_tgid for a group
*
* @param    bcm_ipmc {(input)} IP Mcast entry info 
*
* @returns  BCM error code 
*
* @end
*********************************************************************/
int usl_db_ipmc_port_tgid_set(usl_bcm_ipmc_addr_t *bcm_ipmc);

/*********************************************************************
* @purpose  Update the L2 ports for a group
*
* @param    bcm_ipmc {(input)} IP Mcast entry info 
* @param    cmd      {(input)} Add/Remove ports from L2 bitmap for group
*
* @returns  BCM error code 
*
* @end
*********************************************************************/
int usl_db_ipmc_l2_ports_update(usl_bcm_ipmc_addr_t *ipmc, USL_CMD_t cmd);

/*********************************************************************
* @purpose  Update the L3 ports for a group
*
* @param    bcm_ipmc {(input)} IP Mcast entry info 
* @param    cmd      {(input)} Add/Remove ports from L3 bitmap for group
*
* @returns BCM Error Code
*
* @end
*********************************************************************/
int usl_db_ipmc_l3_ports_update(usl_bcm_ipmc_addr_t *ipmc, USL_CMD_t updateCmd);

/*********************************************************************
* @purpose  Set/Remove a port in L2 bitmap for a number of groups
*
* @param   modid       {(input)}  Modid of the port
* @param   bcmPort     {(input)}  Bcm port number
* @param   cmd         {(input)}  Add/Remove the port to groups
* @param   *index      {(input)}  List of IPMC indexes to modify with this call.
* @param   num_groups  {(input)}  Number of IPMC groups in the *index array.
* @param   vlan_id     {(input)} VLAN affected by this call.
* @param   tagged      {(input)} Flag indicating whether this VLAN should be tagged.
*
*
* @returns  BCM error code 
*
* @end
*********************************************************************/
int usl_db_ipmc_update_l2_port_groups (L7_int32 modid,
                                      L7_int32 bcmPort,
                                      USL_CMD_t updateCmd,
                                      L7_uint32    *ipmc_index,
                                      L7_uint32    num_groups,
                                      L7_uint32    vlan_id,
                                      L7_uint32    tagged);

/*********************************************************************
* @purpose  Set/Remove a port in L3 bitmap for a number of groups
*
* @param   modid           {(input)}  Modid of the port
* @param   bcmPort         {(input)}  Bcm port number
* @param   cmd             {(input)}  Add/Remove the port to groups
* @param   *index          {(input)}  List of IPMC indexes to modify with this call.
* @param   num_groups      {(input)}  Number of IPMC groups in the *index array.
* @param   vlan_id         {(input)}  outgoing VLAN ID.
*
* @returns BCM Error Code
*
* @notes This function adds L3 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
*
* @end
*********************************************************************/
int
usl_db_ipmc_update_l3_port_groups (L7_int32   modid,
                                   L7_int32   bcmPort,
                                   USL_CMD_t  updateCmd,
                                   L7_uint32 *ipmc_index,
                                   L7_uint32  num_groups,
                                   L7_uint32  vlan_id);

/*********************************************************************
*
* @purpose Sets the VLAN replication configuration for a port in an
*          IPMC entry. If there are one or more VLANs replicated, 
*          the port is added to the L3 pbmp of the IPMC entry.
*          If there are zero VLANs replicated, the port is 
*          removed from the IPMC entry.
*
* @param   modid      {(input)} modid of L3 port
* @param   bcmPort    {(input)} BCM port
* @param   ipmcEntry  {(input)} IPMC entry
* @param   vlanVector {(input)} VLAN replication info for L3 port.
*                     
*
* @returns BCM Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_db_ipmc_l3_port_repl_set(L7_uint32 modid, L7_uint32 bcmPort, usl_bcm_ipmc_addr_t *ipmcEntry, bcm_vlan_vector_t vlanVector);

/*********************************************************************
* @purpose  Allocate a hw ipmc index for a given group
*
* @param    groupInfo @{(input)}  L2MC group
* @param    index     @{(output)} Hw Index
*
* @returns  BCM_E_NONE: HwId Generated successfully
*           BCM_E_FULL: No valid HwId exists
*
* @end
*********************************************************************/
int usl_ipmc_hw_id_allocate(usl_bcm_ipmc_addr_t *ipmcAddr, L7_int32 *index);

/*********************************************************************
* @purpose  Free a given ipmc index
*
* @param    index   @{(input)} ipmc index
*
* @returns  BCM_E_NONE: HwId freed successfully
*           BCM_E_FAIL: HwId could not be freed
*
* @end
*********************************************************************/
int usl_ipmc_hw_id_free(L7_int32 index);

/*********************************************************************
* @purpose  Set the IPMC db handle to Operational or Shadow table
*
* @param    dbType  {(input)} Type of db to set
*
* @returns  L7_SUCCESS - if all the ports were deleted
* @returns  L7_FAILURE - An error was encountered during deletion
*
*       
* @end
*********************************************************************/
L7_RC_t usl_ipmc_db_handle_set(USL_DB_TYPE_t dbType);

/*********************************************************************
* @purpose  Set the IPMC db handle to Operational or Shadow table
*
* @param    dbType  {(input)} Type of db to set
*
* @returns  L7_SUCCESS - if all the ports were deleted
* @returns  L7_FAILURE - An error was encountered during deletion
*
*       
* @end
*********************************************************************/
L7_RC_t usl_ipmc_port_db_handle_set(USL_DB_TYPE_t dbType);

/*********************************************************************
* @purpose  Given a dbType, return the db Handle.
*
* @params   dbType            {(input)} OPER/SHADOW/CURRENT
*
* @returns  Pointer to db Handle
*
* @notes    none
*
* @end
*********************************************************************/
avlTree_t *usl_ipmc_port_db_handle_get(USL_DB_TYPE_t dbType);

/*********************************************************************
*
* @purpose Sets the RPF check mode. This function should
*          be called on each stack unit 
*          to disable RPF checking after a management failover occurs.
*          This function will iterate through all IPMC entries in the
*          operational DB and disable RPF checking. This will minimize
*          IPMC data loss during a failover.
*          This function should be called to enable RPF checking before
*          reconciliation occurs so that updated IPMC entries and newly
*          created IPMC entries have RPF checking enabled.
*
* @param   enable     {(input)} RPF checking mode
*
* @returns BCM Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_db_ipmc_rpf_check_mode_set(L7_BOOL enable);

/*********************************************************************
*
* @purpose Gets the RPF check mode.
*
* @param   
*
* @returns L7_BOOL
*
* @notes 
*
* @end
*
*********************************************************************/
L7_BOOL usl_db_ipmc_rpf_check_mode_get();

/*********************************************************************
*
* @purpose Gets the USL info for an IPMC entry based on an IPMC index.
*
* @param   index         {(input)}  IPMC index
*          l7_ipmc       {{output}} IPMC entry data
*
* @returns BCM_E_xxx
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_db_ipmc_entry_get_by_index(L7_uint32 index, usl_bcm_ipmc_addr_t *l7_ipmc);

L7_RC_t usl_db_ipmc_wlan_l3_find_delete(L7_uint32 l3_intf_id, bcm_gport_t wlanvp, 
                                    bcm_vlan_t vlan_id, bcm_if_t *egrintf,
                                    L7_uint32 *use_count, usl_bcm_l3_egress_t *egr,
                                    L7_int32 *self_owned);

L7_RC_t usl_db_ipmc_wlan_l3_find_add(L7_uint32 l3_intf_id, bcm_gport_t wlanvp, 
                                 bcm_vlan_t vlan_id, bcm_if_t *egrintf);

L7_RC_t usl_db_ipmc_wlan_l3_insert(L7_uint32 mc_index, L7_uint32 l3_intf_id, bcm_gport_t wlanvp, 
                                   bcm_vlan_t vlan_id, bcm_if_t egrintf,
                                   usl_bcm_l3_egress_t *egr, L7_int32 self_owned);

L7_RC_t usl_db_ipmc_wlan_l3_delete(L7_uint32 l3_intf_id, bcm_gport_t wlanvp, 
                               bcm_vlan_t vlan_id, bcm_if_t egrintf);

/*********************************************************************
*
* @purpose Cleans up the IPMC DB for failed units.
*
* @param   modids        {{input}}  array of modids
*          modid_count   {{input}}  count of modids in array
*
* @returns BCM_E_xxx
*
* @notes 
*
* @end
*
*********************************************************************/
int usl_ipmc_db_dataplane_cleanup(L7_int32 *modids, L7_uint32 modid_count);

#endif /* L7_MCAST_PACKAGE */
#endif
