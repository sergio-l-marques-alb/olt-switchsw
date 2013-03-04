/******************************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
*******************************************************************************
*
* @filename broad_l3.h
*
* @purpose   Contains external declarations for Broadcom HAPI L3 module
*
* @component HAPI
*
* @comments  Only the functions/macros/variables used by other components in
*            HAPI needs to be declared in this file. Anything used internal to
*            HAPI L3 module need to go in broad_l3_int.h.
*
* @create 10/01/2007
*
* @author sdoke
*
* @end
*
******************************************************************************/
#ifndef BROAD_L3_H_INCLUDED
#define BROAD_L3_H_INCLUDED

#include "bcm/types.h"

#include "dapi.h"
#include "dapi_struct.h"
#include "platform_config.h"

/* Table sizes for AVL trees maintained in HAPI L3 module.
 * If IPv6 package is included, consider both IPv4 and IPv6 routes/hosts 
 */
#ifdef L7_IPV6_PACKAGE
#define HAPI_BROAD_L3_NH_TBL_SIZE      (platRtrArpMaxEntriesGet() + \
                                        platRtrIpv6NdpMaxEntriesGet())
#define HAPI_BROAD_L3_HOST_TBL_SIZE    (platRtrArpMaxEntriesGet() + \
                                        platRtrIpv6NdpMaxEntriesGet())
#define HAPI_BROAD_L3_ROUTE_TBL_SIZE   (platRtrRouteMaxEntriesGet() + \
                                        platRtrIpv6RouteMaxEntriesGet())
#else
#define HAPI_BROAD_L3_NH_TBL_SIZE      (platRtrArpMaxEntriesGet())
#define HAPI_BROAD_L3_HOST_TBL_SIZE    (platRtrArpMaxEntriesGet())
#define HAPI_BROAD_L3_ROUTE_TBL_SIZE   (platRtrRouteMaxEntriesGet())
#endif

/* MAC address table associated with hosts */
#define HAPI_BROAD_L3_MAC_TBL_SIZE     (HAPI_BROAD_L3_HOST_TBL_SIZE)

/* Max ECMP groups - 2K is the size of hardware ECMP table and each ECMP group
 * can have max of L7_RT_MAX_EQUAL_COST_ROUTES entries. ECMP groups are reused
 * for routes having identical ECMP nexthops. So, the table size is derived 
 * based on the max number of different ECMP groups that can be configured
 */
/* Make 2K a platform constant TBD */
#define HAPI_BROAD_L3_MAX_ECMP_GROUPS  (2048/L7_RT_MAX_EQUAL_COST_ROUTES)

/* Max tunnels - includes all types of tunnels (4o4, 6o4, 6to4). There are 
 * different platform constants for 6o4 tunnels (managed by RLIM component)
 * and for 4o4 tunnels (typically used for Wireless). Also note that 6o4
 * tunnels have an interface/USP associated with them. For 4o4/Wireless
 * tunnels there is no associated interface/USP.
 */
#define HAPI_BROAD_L3_MAX_TUNNELS      (L7_MAX_NUM_TUNNEL_INTF + \
                                        L7_L3_TUNNEL_MAX_COUNT)


/* Max number of 6to4 routers reachable via a 6to4 tunnel. It is possibile
 * that every 6to4 router is a relay router as well, in which case we need
 * a explicit 6over4 configured tunnel to each of the relay routers. This
 * constant is derived by considering max 128 tunnel limit in XGS3 (pre-Triumph)
 * and by considering max wireless tunnels
 */
#define HAPI_BROAD_L3_MAX_6TO4_NHOP   16

/* Funtion declarations */

/*******************************************************************************
*
* @purpose Initializes L3 code
*
* @param   *dapi_g system information
*
* @returns L7_SUCCESS
*
* @notes   Any failure during initialization is treated as fatal error.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3Init(DAPI_t *dapi_g);


/*******************************************************************************
*
* @purpose Re-initialize the L3 code.
*
* @param   none
*
* @returns none
*
* @notes   Called from hpcHardwareDriverReset().
*
* @end
*
*******************************************************************************/
void hapiBroadL3ReInit(void);


/*******************************************************************************
*
* @function hapiBroadL3PortInit
*
* @purpose  Hook in DAPI L3 command handlers for the given port
*
* @param    DAPI_PORT_t* dapiPortPtr - generic port instance
*
* @returns  L7_RC_t
*
* @notes    none
*
* @end
*
*******************************************************************************/

L7_RC_t hapiBroadL3PortInit(DAPI_PORT_t *dapiPortPtr);


/*******************************************************************************
*
* @purpose Initialize routing for all ports on this card
*
* @param   L7_ushort16  unitNum - unit number for this card
* @param   L7_ushort16  slotNum - slot number for this card
* @param   DAPI_t *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   Called from hapiBroadPhysicalCardInsert() for physical cards and
*          from hapiBroadL3RouterCardInsert() for logical cards.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3CardInit(L7_ushort16 unitNum,
                            L7_ushort16 slotNum,
                            DAPI_t *dapi_g);


/******************************************************************************
*
* @purpose Removes logical card
*
* @param  dapiUsp         USP for the logical router card
* @param  cmd             DAPI cmd for removing card (not used)
* @param  data            Data associated with the DAPI cmd (not used)
* @param *handle          DAPI driver object handle
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
******************************************************************************/
L7_RC_t hapiBroadL3RouterCardRemove(DAPI_USP_t *usp,
                                     DAPI_CMD_t cmd,
                                     void *data,
                                     DAPI_t *dapi_g);

/******************************************************************************
*
* @purpose Replay global L3 config 
*
* @param *dapi_g          DAPI driver object handle
*
* @returns L7_RC_t result
*
* @notes  Only configuration that is system based is applied/replayed
*
* @end
*
******************************************************************************/
L7_RC_t hapiBroadL3ApplyConfig(DAPI_t *dapi_g);



/******************************************************************************
*
* @purpose Enables/Disables routing globally.
*
* @param  usp         unit/slot/port
* @param  cmd         DAPI_CMD_ROUTING_ROUTE_FORWARDING_CONFIG or
*                     DAPI_CMD_IPV6_ROUTE_FORWARDING_CONFIG
* @param  data        Data associated with the DAPI cmd
* @param *dapi_g      DAPI driver object
*
* @returns L7_RC_t result
*
* @notes This function is also called as part of hapiBroadApplyConfig()
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3RouteFwdConfig(DAPI_USP_t *usp,
                                  DAPI_CMD_t cmd,
                                  void *data,
                                  DAPI_t *dapi_g);

/*******************************************************************************
*
* @purpose Control ICMP redirect to CPU option.
*
* @param  usp     unit/slot/port - not valid for this command
* @param  cmd     DAPI_CMD_ROUTING_ROUTE_ICMP_REDIRECTS_CONFIG
* @param  data    Data associated with ICMP redirect command
* @param *dapi_g  DAPI driver object
*
* @returns L7_RC_t result
*
* @notes This is a global knob only and not per-interface.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3IcmpRedirConfig(DAPI_USP_t *usp,
                                   DAPI_CMD_t cmd,
                                   void *data,
                                   DAPI_t *dapi_g);



/*********************************************************************
*
* @purpose Handles sending the packet when destination is a routing
*          interface
*
* @param  usp       dest unit/slot/port
* @param  frameType frame type, unicast, mcast
* @param  pktData   frame handle
* @param *dapi_g    system information
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t
hapiBroadL3RoutingSend(DAPI_USP_t *usp,
                       DAPI_FRAME_TYPE_t *frameType,
                       L7_uchar8 *pktData,
                       DAPI_t *dapi_g);


/******************************************************************************
*
* @purpose Updates the L2 mac info and updates the nexthop entries if necessary
*
* @param *macAddr   MAC address learnt
* @param  vlanId    VLAN id of the learnt MAC address
* @param  usp       unit/slot/port on which MAC is learnt.
* @param *dapi_g    DAPI driver object
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3UpdateMacLearn(bcm_mac_t macAddr,
                                  L7_ushort16 vlanId,
                                  DAPI_USP_t *usp,
                                  DAPI_t *dapi_g);


/******************************************************************************
*
* @purpose Updates the L2 mac info and updates the nexthop entries if necessary
*
* @param *macAddr   MAC address aged
* @param  vlanId    VLAN ID of the aged MAC address
* @param *dapi_g    system information
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3UpdateMacAge(bcm_mac_t macAddr,
                                L7_ushort16 vlanId,
                                DAPI_t *dapi_g);


/*******************************************************************************
* @purpose  Set the max ECMP paths for a unit
*
* @param    unit - bcm unit number for which ECMP max path needs to be set.
*
* @returns  L7_RC_t
*
* @comments This function needs to be called after bcm_l3_init() is done and
*           before any ECMP routes are configured.
*
* @end
*******************************************************************************/
L7_RC_t hapiBroadL3MaxEcmpPathsSet(L7_uint32 unit);


/******************************************************************************
*
* @purpose Determine if the given MAC destination address is a proxy
*          address for the given VLAN
*
* @param  macAddr     MAC address
* @param  vlanId      VLAN ID
*
* @returns L7_TRUE    If the MAC address is a proxy address
* @returns L7_FALSE   Otherwise
*
* @notes none
*
* @end
*
******************************************************************************/
L7_BOOL hapiBroadL3MacDestIsProxy(L7_uchar8 *macAddr, L7_uint32 vlanId);

/******************************************************************************
*
* @purpose Notification for VLAN port add/delete
*
* @param   DAPI_USP_t  *usp    - unit/slot/port
* @param   L7_ushort16 vlanId  - VLAN Id
* @param   L7_BOOL addDel      - 0 implies port is added to vlan. 1 for delete.
* @param   DAPI_t      *dapi_g
*
* @returns L7_RC_t result
*
* @notes   The callback is used for VLAN routing interfaces. As ports can be
*          added/deleted from routed VLAN, the host policy needs to be updated.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3PortVlanNotify(DAPI_USP_t  *usp,
                                  L7_ushort16 vlanId,
                                  L7_BOOL     addDel,
                                  DAPI_t      *dapi_g);


/*********************************************************************
*
* @purpose During warm start it is possible that some newly learned 
*          unicast and ecmp Next-hop entries could not be inserted in 
*          the USL Db and hardware. This routine triggers the Async
*          task to retry insertion of these failed entries.
*          
*
*
* @returns L7_RC_t result
*
* @notes   Triggers hardware retries of Nhop entries that could not be
*          inserted in the hardware due to temporary table full 
*          condition.
*
* @end
*
*********************************************************************/
void hapiBroadL3WarmStartRetryFailures(void);

#endif  /* BROAD_L3_H_INCLUDED */
